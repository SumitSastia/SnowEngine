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
    DebugMenu::init(window);

    glEnable(GL_MULTISAMPLE);
    glDisable(GL_FRAMEBUFFER_SRGB);

    // ---------- CallBack Functions ---------- //

    glfwSetKeyCallback(window, Input::key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ---------- Testing --------------------- //

    Camera& mainCamera = Camera::instance();

    Scene* mainScene = new Scene1();

    DebugFrame* debugFrame = new DebugFrame(WIN_W, WIN_H);
    HDRFrame*   hdrFrame   = new HDRFrame(WIN_W, WIN_H);
    // BloomFrame* bloomFrame = new BloomFrame(WIN_W, WIN_H);

    Gbuffer* deferredFrame = new Gbuffer(WIN_W, WIN_H);

    bool deferredRender = false;

    // ---------- Loop ------------------------ //

    while (!glfwWindowShouldClose(window) && isRunning) {

        // Time //DebugMenu::endBox();
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        DebugMenu::beginUI();

        // Inputs //
        Input::update();
        
        mainCamera.mouse_handler(window);
        mainCamera.scroll_handler(scrollOffset);
        
        // Events //
        glfwPollEvents();

        if (Input::isKeyDown(GLFW_KEY_P)) {
            deferredRender = !deferredRender;
        }
        
        // Updates // 
        ImGui::Begin("Rendering Method");
        ImGui::TextWrapped((deferredRender)? "Deferred Rendering" : "Forward Rendering");

        mainCamera.input_handler(window, deltaTime);
        mainScene->input(window, deltaTime);
        
        mainCamera.update(deltaTime);
        mainScene->update(deltaTime);

        ImGui::End();

        // Rendering //


        // Scene
        if (!deferredRender) {

            mainScene->renderDirectShadow();
            mainScene->renderPointShadow();
            
            glViewport(0, 0, WIN_W, WIN_H);
            glBindFramebuffer(GL_FRAMEBUFFER, hdrFrame->getFBO());

            Renderer::clear();

            mainScene->render();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            hdrFrame->render();

            Renderer::copyDepth(hdrFrame);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            mainScene->renderLight();
        }
        else {

            mainScene->renderDirectShadow();
            mainScene->renderPointShadow();

            glViewport(0, 0, WIN_W, WIN_H);
            deferredFrame->bindFBO();

            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Renderer::clear();

            mainScene->renderGbuffer();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            Renderer::clear();

            mainScene->renderDeferred(*deferredFrame->getShader());
            deferredFrame->render();

            Renderer::copyDepth(deferredFrame);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            mainScene->renderLight();
        }

        // Debug - UI
        DebugMenu::endUI();

        glfwSwapBuffers(window);
    }

    // ---------- Termination ----------------- //

    mainScene->destroy();

    DebugMenu::instance().destroy();
    Renderer::instance().terminate();

    glfwTerminate();

    return 0;
}