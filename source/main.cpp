#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <string>
#if defined(OPENGL_ENGINE_USE_GLFW) && OPENGL_ENGINE_USE_GLFW
#if __has_include(<GLFW/glfw3.h>)
#include <GLFW/glfw3.h>
#define GLFW_AVAILABLE 1
#elif __has_include(<glfw/glfw3.h>)
#include <glfw/glfw3.h>
#define GLFW_AVAILABLE 1
#else
#define GLFW_AVAILABLE 0
#endif
#else
#define GLFW_AVAILABLE 0
#endif

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
};
Vec2 offset;

void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) // Check if the key was pressed
	    switch (key)
	    {
	    case GLFW_KEY_W:
			offset.y += 0.1f; // Move up
			std::cout << "UP key pressed" << std::endl;
	    	break;
        case GLFW_KEY_S:
			offset.y -= 0.1f; // Move down
               std::cout << "DOWN key pressed"<< std::endl;
               break;
      case GLFW_KEY_D:
              offset.x += 0.1f; // Move right
              std::cout << "RIGHT key pressed"<< std::endl;
          break;
         case GLFW_KEY_A:
                offset.x -= 0.1f; // Move left
                std::cout << "LEFT key pressed"<< std::endl;
         break;
	    default:
		    break;
	}
}

// (Vec2 and offset are declared above so the key callback can use them)


// Callback to update viewport on window resize
static void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}




int main() // Entry point of the application
{
#if !GLFW_AVAILABLE
    std::cerr << "GLFW is not configured. Add its include directory to the project settings.\n";
    return -1;
#else
    if (!glfwInit())
    {
        return -1; 
    }

    // Configure GLFW window hints after initializing GLFW and before creating the window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1080, 720, "Open GL rendering engine", nullptr, nullptr);
    if (window == nullptr) { glfwTerminate(); return -1; }

    // Use the standalone keycallback so key presses (W/A/S/D) are printed to the console.
    glfwSetKeyCallback(window, keycallback);



    // Make the context current BEFORE any OpenGL calls
    glfwMakeContextCurrent(window);

    // Ensure GLEW uses modern techniques for managing OpenGL functionality
    glewExperimental = GL_TRUE;

    // Initialize GLEW AFTER making context current
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

    // Set initial viewport size and register resize callback
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Rectangle made from two triangles, centered at origin
    // Each vertex: position (x,y,z) + color (r,g,b)
    std::vector<float> vertices = {
        -0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // top-left (red)
         0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // top-right (green)
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-right (blue)
        -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f  // bottom-left (yellow)
    };

    // Two triangles: (0,1,2) and (2,3,0)
    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };


    // buffer
    GLuint vbo; 
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray (vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);


    // Each vertex: 3 floats position, 3 floats color -> stride = 6 * sizeof(float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint uOffsetLocation = -1; // will be queried after shader program is linked
   

    // uniform location placeholder (will be set after shader program is linked)
    GLint uColorLocation = -1;

    // Move the window to a visible position
    glfwSetWindowPos(window,500, 150);


    std::string vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform vec2 uOffset; // uniform for offsetting the position



out vec3 vColor; // output a color to the fragment shader

void main()
{
    // add the 2D offset to the x,y components of the position
    gl_Position = vec4(aPos + vec3(uOffset, 0.0), 1.0);
    vColor = aColor;
}
)";




	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSourceCStr = vertexShaderSource.c_str(); // Get the C-style string from std::string
	glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
	glCompileShader(vertexShader); // Compile the vertex shader

	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // Check for compilation errors

    if(!success) 
    {

		char infoLog[512]; // Buffer to hold the error message
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog); // Get the error message
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl; // Print the error message
        

    }

    std::string fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;

in vec3 vColor; // input color from the vertex shader
uniform vec4 uColor; 



void main()
{
    FragColor = vec4(vColor, 1.0f) * uColor;
}
)";


		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str(); // Get the C-style string from std::string
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr); // Set the source code for the fragment shader
		glCompileShader(fragmentShader); // Compile the fragment shader

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // Check for compilation errors

        if (!success)
        {

            char infoLog[512]; // Buffer to hold the error message
            // Use fragmentShader when querying the fragment shader compile log
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog); // Get the error message
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl; // Print the error message


        }
		// Create a shader program and link the vertex and fragment shaders
		GLuint shaderProgram = glCreateProgram(); // Create a shader program
		glAttachShader(shaderProgram, vertexShader); // Attach the vertex shader to the program
        glAttachShader(shaderProgram, fragmentShader); // Attach the fragment shader to the program
		glLinkProgram(shaderProgram); // Link the shader program

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // Check for linking errors

        if(!success)
        {

            char infoLog[512]; // Buffer to hold the error message
			glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog); // Get the error message
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl; // Print the error message

        }

        // Use program and set initial uniform values
        glUseProgram(shaderProgram);
        uColorLocation = glGetUniformLocation(shaderProgram, "uColor");
        if (uColorLocation != -1) {
            // set uniform to white (no tint) by default
            glUniform4f(uColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        // Cache the location of the offset uniform and set initial value
        uOffsetLocation = glGetUniformLocation(shaderProgram, "uOffset");
        if (uOffsetLocation != -1) {
            glUniform2f(uOffsetLocation, offset.x, offset.y);
        }

        glDeleteShader(vertexShader); // Delete the vertex shader as it's no longer needed
        glDeleteShader(fragmentShader); // Delete the fragment shader as it's no longer needed



	while (!glfwWindowShouldClose(window)) // Main render loop
    {
        
        // Use a dark background so the red triangle is visible
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        // If you want to change the tint at runtime, update uColorLocation here
        glBindVertexArray(vao);
        // update offset uniform before drawing
        if (uOffsetLocation != -1) {
            glUniform2f(uOffsetLocation, offset.x, offset.y);
        }
        // EBO is stored in the VAO; no need to bind it each frame, but binding is harmless
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
#endif
}
