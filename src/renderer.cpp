#include <renderer.h>

/*
Returns true if Renderer is successfully initialized.
*/
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

    window = glfwCreateWindow(WIN_W,WIN_H,"FirstWindow 1.0", nullptr, nullptr);

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

    return true;
}

void Renderer::render() const {

    // Background-Color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::terminate() {

}