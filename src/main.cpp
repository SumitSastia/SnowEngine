#include <iostream>

#include <renderer.h>
#include <shader.h>
#include <camera.h>
#include <shapes.h>
#include <debug.h>
#include <input.h>
#include <scenes.h>
#include <model.h>

// ------------------------------ Global Variables ----------------------------------- //

bool isRunning = true;

float deltaTime = 0.0f;
float lastTime  = 0.0f;

// ----------------------------------------------------------------------------------- //

int main() {

    // ---------- Initialization --------------- //
    
    GLFWwindow* window = Renderer::instance().getWindow();

    // Hides the cursor at the Startup
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Input::init();

    glDisable(GL_FRAMEBUFFER_SRGB);

    // ---------- CallBack Functions ---------- //

    glfwSetKeyCallback(window, Input::key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ---------- Testing --------------------- //

    Camera& mainCamera = Camera::instance();
    Debug_menu::instance().init(window);

    Scene* mainScene = new Scene1();

    DebugFrame* debugFrame = new DebugFrame(WIN_W, WIN_H);
    HDRFrame*   hdrFrame   = new HDRFrame(WIN_W, WIN_H);
    // BloomFrame* bloomFrame = new BloomFrame(WIN_W, WIN_H);

    // ---------- Loop ------------------------ //

    while (!glfwWindowShouldClose(window) && isRunning) {

        // Time //
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Inputs //
        Input::update();
        
        mainCamera.mouse_handler(window);
        mainCamera.scroll_handler(scrollOffset);
        
        // Events //
        glfwPollEvents();
        
        // Updates // 
        mainCamera.input_handler(window, deltaTime);
        mainScene->input(window, deltaTime);
        
        mainCamera.update(deltaTime);
        mainScene->update(deltaTime);
        // Debug_menu::instance().update();

        // Rendering //
        mainScene->renderDirectShadow();
        mainScene->renderPointShadow();
        
        glViewport(0, 0, WIN_W, WIN_H);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFrame->getFBO());

        Renderer::instance().render();

        mainScene->render();
        // mainScene->renderDebug();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hdrFrame->render();

        Renderer::copyDepth(hdrFrame->getFBO(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // mainScene->renderLight();
        mainScene->renderSkybox();

        // Debug Menu
        // Debug_menu::instance().render();
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