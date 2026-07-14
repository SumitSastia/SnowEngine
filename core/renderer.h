#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/config.h"

extern std::filesystem::path base;

// ------------------------------ Foward Declarations -------------------------------- //

class FrameBuffer;
class ECS;

struct Transform;
struct EntityMesh;

// ----------------------------------------------------------------------------------- //

class Renderer {

    GLFWmonitor* monitor;
    GLFWwindow*  window;

    int MAX_TEXTURES_SUPPORTED;
    static bool culling;

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

    bool init(); /* Returns true if Renderer is successfully initialized. */
    void render() const {}
    void terminate();
    
    // ------------------------------ Global Render Functions ---------------------------- //

    static void enableCulling();
    static void disableCulling();
    static bool getCullingState() { return culling; }

    static void enableDepth();
    static void disableDepth();

    static void clear();

    static void copyDepth(const FrameBuffer* read_fbo);
    static void copyDepth(unsigned int read_fbo, unsigned int write_fbo);
    static void copyDepth(const FrameBuffer* read_fbo, const FrameBuffer* write_fbo);

    GLFWwindow* getWindow() const { return window; }

};
