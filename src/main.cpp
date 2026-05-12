#include <iostream>

#include <renderer.h>
#include <shader.h>
#include <camera.h>
#include <shapes.h>
#include <debug.h>
#include <input.h>
#include <scenes.h>

// ------------------------------ Global Variables ----------------------------------- //

const bool defaultFBO = 0;

bool isRunning = true;

float deltaTime = 0.0f;
float lastTime  = 0.0f;

// ----------------------------------------------------------------------------------- //

int main() {

    // ---------- Initialization --------------- //
    
    GLFWwindow* window = Renderer::instance().getWindow();

    // Hides the cursor at the Startup
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ---------- CallBack Functions ---------- //

    glfwSetKeyCallback(window, input_key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ---------- Testing --------------------- //

    Camera& mainCamera = Camera::instance();
    Debug_menu::instance().init(window);

    Scene* mainScene = new Scene1();

    DebugFrame* debugFrame = new DebugFrame(WIN_W, WIN_H);

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
        mainCamera.scroll_handler(scrollOffset);

        mainScene->input(window, deltaTime);
        
        // Updates // 
        mainCamera.update(deltaTime);
        mainScene->update(deltaTime);
        // Debug_menu::instance().update();

        // Rendering //
        mainScene->renderDirectShadow();
        mainScene->renderPointShadow();
        
        glViewport(0, 0, WIN_W, WIN_H);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        // glBindFramebuffer(GL_FRAMEBUFFER, debugFrame->getFBO());

        Renderer::instance().render();

        mainScene->render();
        // mainScene->renderDebug();

        // Debug Menu
        // Debug_menu::instance().render();

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // debugFrame->render();

        glfwSwapBuffers(window);
    }

    // ---------- Termination ----------------- //

    mainScene->destroy();

    Debug_menu::instance().destroy();
    Renderer::instance().terminate();

    glfwTerminate();

    return 0;
}