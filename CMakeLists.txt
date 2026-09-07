cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project (GameDevelopmentProject)

set(PROJECT_SOURCE_FILES
  source/main.cpp
)
# Add the application
include_directories(source)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

# Enable GLFW in source via a preprocessor definition so the code includes GLFW headers
target_compile_definitions(${PROJECT_NAME} PRIVATE OPENGL_ENGINE_USE_GLFW=1)

# Add GLFW library
add_subdirectory(thirdparty/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
include_directories(thirdparty/glfw-3.4/include)

# Add Glew library
add_compile_definitions(GLEW_NO_GLU)
set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
add_subdirectory(thirdparty/glew/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
include_directories(thirdparty/glew/include)
set_property(TARGET glew_s PROPERTY FOLDER GLEW)
set_property(TARGET glew PROPERTY FOLDER GLEW)

# Link all thirdparty libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    glfw 
    glew_s
)

# On Windows link to system OpenGL library
if (WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()
