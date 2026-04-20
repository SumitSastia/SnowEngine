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

public:

    static Renderer& instance() {
        
        static Renderer instance;
        return instance;
    }

    bool init();
    void render() const;
    void terminate();

    GLFWwindow* getWindow() const { return window; }

};