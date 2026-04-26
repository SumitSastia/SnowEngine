#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

const unsigned int WIN_W = 1120;
const unsigned int WIN_H = 700;

class Renderer {

    GLFWmonitor* monitor;
    GLFWwindow* window;

    Renderer() { 
        
        if (!init()) {
            std::cerr << "Unable to initialize the Renderer!" << std::endl;
        }
    }

    static std::string loadShaderFile(const char* path);

public:

    static Renderer& instance() {
        
        static Renderer instance;
        return instance;
    }

    bool init();
    void render() const;
    void terminate();

    static const unsigned int createShader(const char* vertPath, const char* fragPath);
    static const unsigned int createShader2(const char* vertPath, const char* geomPath, const char* fragPath);

    static void renderShadow(const u_int& VAO, const u_int& indicesCount);
    static void renderShadowInstanced(const u_int& VAO, const u_int& indicesCount, const u_int& instanceCounts);

    GLFWwindow* getWindow() const { return window; }

};