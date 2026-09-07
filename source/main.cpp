#include <iostream>
#include <GL/glew.h>
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


int main()
{


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#if !GLFW_AVAILABLE
    std::cerr << "GLFW is not configured. Add its include directory to the project settings.\n";
    return -1;




#else
    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1080, 720, "Open GL rendering Window", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }


    // Move the window to a visible position
    glfwSetWindowPos(window,500, 150);
    glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
    {
		glfwTerminate();
		return -1;
    }





while (!glfwWindowShouldClose(window))
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