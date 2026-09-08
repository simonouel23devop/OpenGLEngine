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

    GLFWwindow* window = glfwCreateWindow(1080, 720, "Open GL rendering Window", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }

	std::vector <float> vertices = {
		0.0f, 0.5f, 0.0f, // Bottom-left vertex
		 -0.5f, -0.5f, 0.0f, // Bottom-right vertex
		 0.0f,  -0.5f, 0.0f  // Top vertex
	};


    // Move the window to a visible position
    glfwSetWindowPos(window,500, 150);
    glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
    {
		glfwTerminate();
		return -1;
    }


	std::string vertexShaderSource = R"( // Vertex shader source code
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main()
        {
            gl_Position = vec4 (aPos.x, aPos.y , aPos.z, 1.0);
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

    std::string fragmentShaderSource = R"( // Fragment shader source code
        #version 330 core
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        })";


		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str(); // Get the C-style string from std::string
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr); // Set the source code for the fragment shader
		glCompileShader(fragmentShader); // Compile the fragment shader

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // Check for compilation errors

        if (!success)
        {

            char infoLog[512]; // Buffer to hold the error message
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog); // Get the error message
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl; // Print the error message


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
        
	    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
#endif
} 