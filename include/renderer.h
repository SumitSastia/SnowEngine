#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>

const unsigned int WIN_W = 1120;
const unsigned int WIN_H = 700;

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

    GLFWwindow* getWindow() const { return window; }

};

class Texture2D {

    int width;
    int height;
    int nrChannels;
    
    std::string type;
    unsigned int textureID;
    unsigned char* pixelData;
    
public:

    Texture2D() :
        width(0),
        height(0),
        nrChannels(0),
        textureID(0) {
    }

    /*
    Loads the image and allocate it into the Memory.
    NOTE: "path" should start with '/' and rest should continue after the Main Directory (/SnowEngine).
    */
    void load(const char* path);

    void destroy();

    const unsigned int& getID()  const { return textureID; }
    const std::string& getType() const { return type; }
};