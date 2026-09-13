#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

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

// Grid dimensions
#define GRID_W 20
#define GRID_H 15

struct Cell { int x; int y; };

static void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

static bool cellEquals(const Cell &a, const Cell &b) { return a.x==b.x && a.y==b.y; }

static Cell randFood(const std::vector<Cell>& snake) {
    Cell f;
    bool ok = false;
    while(!ok) {
        f.x = rand() % GRID_W;
        f.y = rand() % GRID_H;
        ok = true;
        for (auto &s: snake) if (cellEquals(s,f)) { ok = false; break; }
    }
    return f;
}

int main() {
#if !GLFW_AVAILABLE
    std::cerr << "GLFW is not configured. Add its include directory to the project settings.\n";
    return -1;
#else
    srand((unsigned)time(nullptr));
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1080, 720, "Snake game - OpenGL", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { glfwTerminate(); return -1; }

    int fbW, fbH; glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0,0,fbW,fbH);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // unit quad
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    std::vector<unsigned int> indices = {0,1,2, 2,3,0};

    GLuint vao, vbo, ebo; glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo); glGenBuffers(1,&ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0); glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    const char* vsrc = R"(#version 330 core
layout(location=0) in vec3 aPos;
uniform vec2 uOffset;
uniform vec2 uScale;
void main(){ vec2 p = aPos.xy * uScale + uOffset; gl_Position = vec4(p,0.0,1.0); })";
    const char* fsrc = R"(#version 330 core
out vec4 FragColor; uniform vec4 uColor; void main(){ FragColor = uColor; })";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs,1,&vsrc,nullptr); glCompileShader(vs);
    GLint ok; glGetShaderiv(vs, GL_COMPILE_STATUS, &ok); if(!ok){char buf[512]; glGetShaderInfoLog(vs,512,nullptr,buf); std::cerr<<"VS:"<<buf<<"\n";}
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs,1,&fsrc,nullptr); glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok); if(!ok){char buf[512]; glGetShaderInfoLog(fs,512,nullptr,buf); std::cerr<<"FS:"<<buf<<"\n";}
    GLuint prog = glCreateProgram(); glAttachShader(prog,vs); glAttachShader(prog,fs); glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &ok); if(!ok){char buf[512]; glGetProgramInfoLog(prog,512,nullptr,buf); std::cerr<<"LINK:"<<buf<<"\n";}
    glDeleteShader(vs); glDeleteShader(fs);

    GLint locOffset = glGetUniformLocation(prog,"uOffset");
    GLint locScale = glGetUniformLocation(prog,"uScale");
    GLint locColor = glGetUniformLocation(prog,"uColor");

    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // helper to draw rectangles (center x,y in NDC, width/height in NDC)
    auto drawRect = [&](float cx, float cy, float w, float h, float r, float g, float b, float a){
        glUniform2f(locScale, w, h);
        glUniform2f(locOffset, cx, cy);
        glUniform4f(locColor, r, g, b, a);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    };

    // draw a single digit (0-9) using 5 stroke rectangles in a letter box
    auto drawDigit = [&](int d, float x, float y, float w, float h, float r, float g, float b){
        float th = h * 0.12f; // thickness
        float hx = w*0.5f; float hy = h*0.5f;
        // positions of strokes relative to center
        // top
        if (d!=1 && d!=4) drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f);
        // bottom
        if (d!=1 && d!=4 && d!=7) drawRect(x, y - hy + th*0.5f, w - th, th, r,g,b,1.0f);
        // left
        if (d!=2 && d!=3 && d!=7) drawRect(x - hx + th*0.5f, y, th, h - 2*th, r,g,b,1.0f);
        // right
        if (d!=5 && d!=6) drawRect(x + hx - th*0.5f, y, th, h - 2*th, r,g,b,1.0f);
        // middle
        if (d==2 || d==3 || d==4 || d==5 || d==6 || d==8 || d==9) drawRect(x, y, w - th, th, r,g,b,1.0f);
    };

    // draw a few uppercase letters needed: P A U S C R O E D
    auto drawLetter = [&](char c, float x, float y, float w, float h, float r, float g, float b){
        float th = h * 0.12f;
        float hx = w*0.5f; float hy = h*0.5f;
        switch(c){
            case 'P':
                drawRect(x - hx + th*0.5f, y, th, h - th, r,g,b,1.0f); // left
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f); // top
                drawRect(x, y, w - th, th, r,g,b,1.0f); // mid
                drawRect(x + hx - th*0.5f, y + hy*0.5f, th, h*0.5f - th, r,g,b,1.0f); // right upper
                break;
            case 'A':
                drawRect(x - hx + th*0.5f, y, th, h - th, r,g,b,1.0f); // left
                drawRect(x + hx - th*0.5f, y, th, h - th, r,g,b,1.0f); // right
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f); // top
                drawRect(x, y, w - th, th, r,g,b,1.0f); // middle
                break;
            case 'U':
                drawRect(x - hx + th*0.5f, y, th, h - th, r,g,b,1.0f);
                drawRect(x + hx - th*0.5f, y, th, h - th, r,g,b,1.0f);
                drawRect(x, y - hy + th*0.5f, w - th, th, r,g,b,1.0f);
                break;
            case 'S':
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f);
                drawRect(x - hx + th*0.5f, y + hy*0.5f, th, h*0.5f - th, r,g,b,1.0f);
                drawRect(x, y, w - th, th, r,g,b,1.0f);
                drawRect(x + hx - th*0.5f, y - hy*0.5f, th, h*0.5f - th, r,g,b,1.0f);
                drawRect(x, y - hy + th*0.5f, w - th, th, r,g,b,1.0f);
                break;
            case 'C':
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f);
                drawRect(x - hx + th*0.5f, y, th, h - 2*th, r,g,b,1.0f);
                drawRect(x, y - hy + th*0.5f, w - th, th, r,g,b,1.0f);
                break;
            case 'R':
                drawRect(x - hx + th*0.5f, y, th, h - th, r,g,b,1.0f); // left
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f); // top
                drawRect(x, y, w - th, th, r,g,b,1.0f); // mid
                drawRect(x + hx - th*0.5f, y - hy*0.0f, th, h*0.5f - th, r,g,b,1.0f); // right upper
                drawRect(x + hx*0.0f, y - hy*0.25f, th, th, r,g,b,1.0f); // small leg (approx)
                break;
            case 'O':
                drawRect(x - hx + th*0.5f, y, th, h - 2*th, r,g,b,1.0f);
                drawRect(x + hx - th*0.5f, y, th, h - 2*th, r,g,b,1.0f);
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f);
                drawRect(x, y - hy + th*0.5f, w - th, th, r,g,b,1.0f);
                break;
            case 'E':
                drawRect(x - hx + th*0.5f, y, th, h - th, r,g,b,1.0f);
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f);
                drawRect(x, y, w - th, th, r,g,b,1.0f);
                drawRect(x, y - hy + th*0.5f, w - th, th, r,g,b,1.0f);
                break;
            case 'D':
                drawRect(x - hx + th*0.5f, y, th, h - 2*th, r,g,b,1.0f);
                drawRect(x + hx - th*0.5f, y, th, h - 2*th, r,g,b,1.0f);
                drawRect(x, y + hy - th*0.5f, w - th, th, r,g,b,1.0f);
                drawRect(x, y - hy + th*0.5f, w - th, th, r,g,b,1.0f);
                break;
        }
    };

    // game state
    std::vector<Cell> snake;
    snake.push_back({GRID_W/2, GRID_H/2});
    snake.push_back({GRID_W/2-1, GRID_H/2});
    snake.push_back({GRID_W/2-2, GRID_H/2});
    Cell food = randFood(snake);
    int dirX = 1, dirY = 0;
    int nextDirX = dirX, nextDirY = dirY;

    // pause & score
    bool paused = false;
    int score = 0;
    bool prevP = false; // previous state of pause key

    float cellW = 2.0f / (float)GRID_W;
    float cellH = 2.0f / (float)GRID_H;
    float scaleX = cellW * 0.9f;
    float scaleY = cellH * 0.9f;

    double last = glfwGetTime();
    double accumulator = 0.0;
    const double step = 0.12; // seconds per move

    while(!glfwWindowShouldClose(window)){
        double now = glfwGetTime(); double dt = now - last; last = now; accumulator += dt;

        // input (poll and set next direction)
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
        // toggle pause on P press (edge detect)
        bool pPressed = (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
        if (pPressed && !prevP) { paused = !paused; }
        prevP = pPressed;
        // if paused, ignore movement keys
        if (!paused) {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){ if (!(dirY==1 && dirX==0)) { nextDirX=0; nextDirY=-1; }}
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){ if (!(dirY==-1 && dirX==0)) { nextDirX=0; nextDirY=1; }}
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){ if (!(dirX==1 && dirY==0)) { nextDirX=-1; nextDirY=0; }}
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){ if (!(dirX==-1 && dirY==0)) { nextDirX=1; nextDirY=0; }}
        }

        if (!paused && accumulator >= step){
            accumulator -= step;
            dirX = nextDirX; dirY = nextDirY;
            Cell head = snake.front();
            Cell newHead{ head.x + dirX, head.y + dirY };
            // wall collision
            if (newHead.x < 0 || newHead.x >= GRID_W || newHead.y < 0 || newHead.y >= GRID_H){
                // reset
                snake.clear(); snake.push_back({GRID_W/2, GRID_H/2}); snake.push_back({GRID_W/2-1, GRID_H/2}); snake.push_back({GRID_W/2-2, GRID_H/2});
                dirX=1; dirY=0; nextDirX=1; nextDirY=0; food = randFood(snake);
                score = 0;
            } else {
                // self collision
                bool collided = false; for (auto &s: snake) if (cellEquals(s,newHead)) { collided = true; break; }
                if (collided){ snake.clear(); snake.push_back({GRID_W/2, GRID_H/2}); snake.push_back({GRID_W/2-1, GRID_H/2}); snake.push_back({GRID_W/2-2, GRID_H/2}); dirX=1; dirY=0; nextDirX=1; nextDirY=0; food = randFood(snake); score = 0; }
                else {
                    snake.insert(snake.begin(), newHead);
                    if (cellEquals(newHead, food)) { food = randFood(snake); score++; }
                    else { snake.pop_back(); }
                }
            }
        }

        // render
        glClearColor(0.08f,0.08f,0.08f,1.0f); glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog); glBindVertexArray(vao);
        // draw food
        float fx = -1.0f + cellW * (food.x + 0.5f);
        float fy =  1.0f - cellH * (food.y + 0.5f);
        glUniform2f(locScale, scaleX, scaleY); glUniform2f(locOffset, fx, fy); glUniform4f(locColor, 0.9f,0.2f,0.2f,1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // draw snake
        for (size_t i=0;i<snake.size();++i){
            auto &s = snake[i];
            float x = -1.0f + cellW * (s.x + 0.5f);
            float y =  1.0f - cellH * (s.y + 0.5f);
            if (i==0) glUniform4f(locColor, 0.1f,0.8f,0.1f,1.0f); else glUniform4f(locColor, 0.05f,0.6f,0.05f,1.0f);
            glUniform2f(locScale, scaleX, scaleY); glUniform2f(locOffset, x, y);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // draw pause overlay/menu if paused
        if (paused) {
            // translucent full-screen overlay
            glUniform2f(locScale, 2.0f, 2.0f);
            glUniform2f(locOffset, 0.0f, 0.0f);
            glUniform4f(locColor, 0.0f, 0.0f, 0.0f, 0.6f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // centered menu box
            glUniform2f(locScale, 0.8f, 0.4f);
            glUniform2f(locOffset, 0.0f, 0.0f);
            glUniform4f(locColor, 0.15f, 0.15f, 0.15f, 0.95f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Draw PAUSED text centered
            float letterW = 0.12f; float letterH = 0.2f; float spacing = 0.02f;
            std::string pausedText = "PAUSED";
            float totalW = pausedText.size() * letterW + (pausedText.size()-1) * spacing;
            float startX = -totalW/2.0f + letterW/2.0f;
            for (size_t i=0;i<pausedText.size();++i){
                drawLetter(pausedText[i], startX + i*(letterW+spacing), 0.0f, letterW, letterH, 1.0f, 1.0f, 1.0f);
            }
        }

        // draw score in top-left
        {
            int temp = score;
            std::string scoreStr = std::to_string(temp);
            float letterW = 0.06f; float letterH = 0.12f; float spacing = 0.01f;
            float startX = -1.0f + 0.05f + letterW/2.0f;
            float startY = 1.0f - 0.08f - letterH/2.0f;
            // draw label "SCORE"
            std::string label = "SCORE";
            for (size_t i=0;i<label.size();++i){ drawLetter(label[i], startX + i*(letterW+spacing), startY, letterW, letterH, 1.0f,1.0f,0.4f); }
            // draw digits to the right
            float digitsX = startX + (label.size())*(letterW+spacing) + 0.02f;
            for (size_t i=0;i<scoreStr.size();++i){ int d = scoreStr[i]-'0'; drawDigit(d, digitsX + i*(letterW+spacing), startY, letterW, letterH, 1.0f,1.0f,1.0f); }
        }

        // update window title with score and pause state
        {
            std::string title = std::string("Snake - Score: ") + std::to_string(score) + (paused ? " (PAUSED) - Press P to resume" : " - Press P to pause");
            glfwSetWindowTitle(window, title.c_str());
        }

        glfwSwapBuffers(window);
    }

    glDeleteProgram(prog); glDeleteBuffers(1,&vbo); glDeleteBuffers(1,&ebo); glDeleteVertexArrays(1,&vao);
    glfwDestroyWindow(window); glfwTerminate();
    return 0;
#endif
}
