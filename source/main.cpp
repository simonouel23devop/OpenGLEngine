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

    // Equilateral triangle centered at the origin (centroid at 0,0)
    // Height h = sqrt(3)/2; top vertex y = 2h/3 ~ 0.57735, base y = -h/3 ~ -0.288675
    std::vector<float> vertices = {
        0.0f,  0.57735026919f, 0.0f,  // top vertex
       -0.5f, -0.28867513459f, 0.0f,  // bottom-left vertex
        0.5f, -0.28867513459f, 0.0f   // bottom-right vertex
    };


    // buffer
    GLuint vbo; 
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray (vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // Move the window to a visible position
    glfwSetWindowPos(window,500, 150);


    std::string vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
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
void main()
{
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
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

		glDeleteShader(vertexShader); // Delete the vertex shader as it's no longer needed
		glDeleteShader(fragmentShader); // Delete the fragment shader as it's no longer needed



	while (!glfwWindowShouldClose(window)) // Main render loop
    {
        
        // Use a dark background so the red triangle is visible
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
#endif
}
