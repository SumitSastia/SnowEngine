#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

const unsigned int WIN_W = 1280;
const unsigned int WIN_H = 720;

extern std::filesystem::path base;

class Renderer {

    GLFWmonitor* monitor;
    GLFWwindow*  window;

    int MAX_TEXTURES;

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
    
    // ------------------------------ Global Render Functions ---------------------------- //

    static void enableCulling();
    static void disableCulling();

    static void enableDepth();
    static void disableDepth();

    static void copyDepth(unsigned int fbo1, unsigned int fbo2);

    GLFWwindow* getWindow() const { return window; }

};