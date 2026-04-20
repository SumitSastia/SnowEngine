#include <iostream>

#include <renderer.h>
#include <shader.h>
#include <camera.h>
#include <shapes.h>

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
    
    GLFWwindow* window = Renderer::instance().getWindow();

    // Hides the cursor at the Startup
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ---------- CallBack Functions ---------- //

    glfwSetKeyCallback(window, input_key_callback);

    // ---------- Testing --------------------- //

    camera& mainCamera = camera::instance();

    unsigned int cubeShader = createShader(
        "../shaders/normalCube/cube.vert",
        "../shaders/normalCube/cube.frag"
    );

    unsigned int planeShader = createShader(
        "../shaders/planes/plane.vert",
        "../shaders/planes/plane.frag"
    );

    const unsigned int shader = planeShader;

    specShape myCube = defaultShapes::instance().cube;
    shape plane      = defaultShapes::instance().square; 

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
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        Renderer::instance().render();

        glUseProgram(shader);

        setMat4(shader, "projection", mainCamera.getPerspective());
        setMat4(shader, "view"      , mainCamera.getView());

        // glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
        // glBindVertexArray(0);

        glm::mat4 objectModel(1.0f);
        // objectModel = glm::scale(objectModel, glm::vec3(0.2f));

        setMat4(shader, "model", objectModel);

        plane.draw();

        // myCube.draw();

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