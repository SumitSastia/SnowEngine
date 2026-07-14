#include "renderer.h"
#include "core/frame/frame.h"

bool Renderer::culling = false;

bool Renderer::init() {

    if (!glfwInit()) {
        std::cerr << "Unable to initialize GLFW!" << std::endl;
        return false;
    }

    // OpenGL Configurations //
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);


    // ---------- Monitor --------------------- //

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    
    if(!monitor){
        std::cerr << "Unable to fetch the Monitor!" << std::endl;
        return false;
    }
    
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

    // ---------- Main Window ----------------- //

    window = glfwCreateWindow(WIN_W,WIN_H,"SnowEngine 1.0", nullptr, nullptr);

    if(!window){
        std::cerr << "Unable to initialize the window!" << std::endl;
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if(!gladLoadGL()){
        std::cerr << "Unable to Load OpenGL!" << std::endl;
        return -1;
    }

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    const unsigned int win_Xpos = videoMode->width/2 - WIN_W/2;
    const unsigned int win_Ypos = videoMode->height/2 - WIN_H/2;

    glfwSetWindowPos(window,win_Xpos,win_Ypos);
    glfwSetWindowAttrib(window,GLFW_RESIZABLE,GLFW_FALSE);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    glViewport(0,0,frameWidth,frameHeight);
    
    glfwSwapInterval(1); // V-Sync

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // Max Textures that can be Loaded by GPU = 32
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MAX_TEXTURES_SUPPORTED);

    return true;
}

void Renderer::terminate() {
    glfwDestroyWindow(window);
}

void Renderer::enableCulling() {

    glEnable(GL_CULL_FACE);
    culling = true;
}

void Renderer::disableCulling() {

    glDisable(GL_CULL_FACE);
    culling = false;
}

void Renderer::enableDepth() {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::disableDepth() {
    glDisable(GL_DEPTH_TEST);
}

void Renderer::clear() {

    // Background-Color
    glClearColor(0.006f, 0.006f, 0.006f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::copyDepth(const FrameBuffer* read_fbo) {

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo->getFBO());  // source
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // destination
    
    glBlitFramebuffer(
        0, 0, WIN_W, WIN_H,
        0, 0, WIN_W, WIN_H,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );
}

void Renderer::copyDepth(unsigned int read_fbo, unsigned int write_fbo) {

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo);  // source
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, write_fbo); // destination
    
    glBlitFramebuffer(
        0, 0, WIN_W, WIN_H,
        0, 0, WIN_W, WIN_H,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );
}

void Renderer::copyDepth(const FrameBuffer* read_fbo, const FrameBuffer* write_fbo) {

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo->getFBO());  // source
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, write_fbo->getFBO()); // destination
    
    glBlitFramebuffer(
        0, 0, WIN_W, WIN_H,
        0, 0, WIN_W, WIN_H,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );
}

