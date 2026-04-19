#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <interface.h>
#include <shader.h>
#include <camera.h>

// ------------------------------ Global Variables ----------------------------------- //

const bool defaultFBO = 0;

bool isRunning     = true;
bool mouseInCamera = true;

float deltaTime = 0.0f;
float lastTime  = 0.0f;

float cursor_dx = 0.0f;
float cursor_dy = 0.0f;

// ------------------------------ Function Delcarations ------------------------------ //

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void track_mouse(GLFWwindow* window);

// ----------------------------------------------------------------------------------- //

int main() {

    // ---------- Initialization --------------- //

    if (!glfwInit()) {
        std::cerr << "Unable to initialize GLFW!" << std::endl;
        return -1;
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
        return -1;
    }
    
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

    // ---------- Main Window ----------------- //

    GLFWwindow* window = glfwCreateWindow(WIN_W,WIN_H,"FirstWindow 1.0", nullptr, nullptr);
    
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

    // ---------- CallBack Functions ---------- //

    glfwSetKeyCallback(window, input_key_callback);

    // ---------- Testing --------------------- //

    camera& mainCamera = camera::instance();

    const float vertices[] = {

        // Front
        -0.5, 0.5, 0.5,  0.0,0.0, 1.0, 0.0,0.0,
         0.5, 0.5, 0.5,  0.0,0.0, 1.0, 1.0,0.0,
        -0.5,-0.5, 0.5,  0.0,0.0, 1.0, 0.0,1.0,
         0.5,-0.5, 0.5,  0.0,0.0, 1.0, 1.0,1.0,
         
        // Back
         0.5, 0.5,-0.5,  0.0,0.0,-1.0, 0.0,0.0,
        -0.5, 0.5,-0.5,  0.0,0.0,-1.0, 1.0,0.0,
         0.5,-0.5,-0.5,  0.0,0.0,-1.0, 0.0,1.0,
        -0.5,-0.5,-0.5,  0.0,0.0,-1.0, 1.0,1.0,

        // Left
        -0.5, 0.5,-0.5,  -1.0,0.0,0.0, 0.0,0.0,
        -0.5, 0.5, 0.5,  -1.0,0.0,0.0, 1.0,0.0,
        -0.5,-0.5,-0.5,  -1.0,0.0,0.0, 0.0,1.0,
        -0.5,-0.5, 0.5,  -1.0,0.0,0.0, 1.0,1.0,

        // Right
         0.5, 0.5, 0.5,   1.0,0.0,0.0, 0.0,0.0,
         0.5, 0.5,-0.5,   1.0,0.0,0.0, 1.0,0.0,
         0.5,-0.5, 0.5,   1.0,0.0,0.0, 0.0,1.0,
         0.5,-0.5,-0.5,   1.0,0.0,0.0, 1.0,1.0,

        // Top
        -0.5, 0.5,-0.5,  0.0, 1.0,0.0, 0.0,0.0,
         0.5, 0.5,-0.5,  0.0, 1.0,0.0, 1.0,0.0,
        -0.5, 0.5, 0.5,  0.0, 1.0,0.0, 0.0,1.0,
         0.5, 0.5, 0.5,  0.0, 1.0,0.0, 1.0,1.0,

        // Bottom
         0.5,-0.5,-0.5,  0.0,-1.0,0.0, 0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,-1.0,0.0, 1.0,0.0,
         0.5,-0.5, 0.5,  0.0,-1.0,0.0, 0.0,1.0,
        -0.5,-0.5, 0.5,  0.0,-1.0,0.0, 1.0,1.0
    };

    unsigned int indices[] = {

        // Front
        0,1,2,
        1,3,2,

        // Back
        4,5,6,
        5,7,6,

        // Left
        8,9,10,
        9,11,10,

        // Right
        12,13,14,
        13,15,14,

        // Top
        16,17,18,
        17,19,18,

        // Bottom
        20,21,22,
        21,23,22
    };

    unsigned int VBO, VAO, EBO;

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    unsigned int shader = createShader(
        "../shaders/cube.vert",
        "../shaders/cube.frag"
    );

    

    // ---------- Loop ------------------------ //

    while (!glfwWindowShouldClose(window) && isRunning) {

        // Time //
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Events //
        glfwPollEvents();
        
        // Inputs //
        mainCamera.input_handler(window, deltaTime);
        mainCamera.mouse_handler(window);
        
        // Updates // 
        mainCamera.update(deltaTime);

        // Rendering //
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Background-Color

        glUseProgram(shader);

        setMat4(shader, "projection", mainCamera.getPerspective());
        setMat4(shader, "view"      , mainCamera.getView());
        setMat4(shader, "model"     , glm::mat4(1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // ---------- Termination ----------------- //

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// ------------------------------ Function Definitions ------------------------------- //

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if(glfwGetKey(window, GLFW_KEY_ESCAPE)){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if(glfwGetKey(window, GLFW_KEY_E)){

        if(!mouseInCamera) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else               glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        mouseInCamera = !mouseInCamera;
    }
}

void track_mouse(GLFWwindow* window){

    static bool mouse_click_hold = false;

    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {

        cursor_dx = 0.0f;
        cursor_dy = 0.0f;
        mouse_click_hold = false;
        return;
    }

    double pos_x;
    double pos_y;

    glfwGetCursorPos(window, &pos_x, &pos_y);

    static double prev_x = pos_x;
    static double prev_y = pos_y;

    if(!mouse_click_hold) {
        prev_x = pos_x;
        prev_y = pos_y;
        mouse_click_hold = true;
    }

    cursor_dx = pos_x - prev_x;
    cursor_dy = pos_y - prev_y;

    prev_x = pos_x;
    prev_y = pos_y;
}