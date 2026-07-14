#include <iostream>

#include "core/camera.h"
#include "core/renderer.h"
#include "core/input.h"
#include "core/text.h"

#include "core/model/model.h"
#include "core/deferred/ssao.h"
#include "core/img/assetManager.h"

#include "core/shader.h"
#include "core/shader/uniforms.h"

#include "scenes/scenes.h"

#include "utils/crosshair.h"
#include "utils/debug.h"

// ------------------------------ Global Variables ----------------------------------- //

bool isRunning = true;
bool isPaused  = false;

float deltaTime = 0.0f;
float lastTime  = 0.0f;

// ----------------------------------------------------------------------------------- //

int main() {

    // ---------- Initialization --------------- //

    running::core::GlobalTimer.restart();
    std::cout << "//----------------------------------------//\n\n"; 
    
    GLFWwindow* window = Renderer::instance().getWindow();

    // Hides the cursor at the Startup
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Input::init();
    DebugMenu::init(window);

    // Text::init("../assets/fonts/BlockBlueprint.ttf");
    Text::init("../assets/fonts/Lato-Bold.ttf");

    ShaderManager::initShaders();
    AssetManager::init();
    
    Line::init();
    Crosshair::init();
    SSAO::init();

    gfx::particles::init();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glDisable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---------- CallBack Functions ---------- //

    glfwSetKeyCallback(window, Input::key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, Input::mouseButton_callback);

    // ---------- Testing --------------------- //

    Scene2* scene2 = new Scene2();

    // BloomFrame* bloomFrame = new BloomFrame(WIN_W, WIN_H);

    DepthFrame depthFrame {};
    depthFrame.create(WIN_W, WIN_H);
    
    HDRFrame* hdrFrame      = new HDRFrame(WIN_W, WIN_H);
    Gbuffer*  deferredFrame = new Gbuffer(WIN_W, WIN_H);

    bool deferredRender = false;

    UBOhandler uboHandler {};
    uboHandler.init();

    DebugMenu::log("Time taken to Initialize: " + running::core::GlobalTimer.end());

    // ---------- Loop ------------------------ //

    float update_timer = 0.0f;
    float fps = 0.0f, frameTime = 0.0f;

    while (!glfwWindowShouldClose(window) && isRunning) {

        // Time
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime  = currentTime;

        update_timer += deltaTime;

        if (update_timer >= 0.5f) {

            fps = 1.0f / deltaTime;
            frameTime = deltaTime * 1000.0f;
            update_timer = 0.0f;
        }

        DebugMenu::beginUI();

        // Events //
        Input::update();
        glfwPollEvents();

        // Inputs //
        if (Input::isKeyDown(GLFW_KEY_H)) {
            SSAO::enable = !SSAO::enable;
        }

        if (Input::isKeyDown(GLFW_KEY_C)) {
            Camera::activeCamera = &Camera::instance();
        }

        scene2->input(window, deltaTime);

        Camera::activeCamera->handle_mouse(window);
        
        if (Input::isKeyDown(GLFW_KEY_U)) {
            deferredRender = !deferredRender;
        }

        if (Input::isKeyDown(GLFW_KEY_P)) {
            isPaused = !isPaused;
        }
        
        // Updates // 
        ImGui::Begin("Rendering Method");
        ImGui::TextWrapped((deferredRender)? "Deferred Rendering" : "Forward Rendering");

        DefaultLights::instance().update();
        Camera::activeCamera->input(window, deltaTime);

        ImGui::End();

        Camera::activeCamera->update(deltaTime);
        uboHandler.update();
        
        if (!isPaused) {
            scene2->update(deltaTime);
        }

        if (Input::isKeyPressed(GLFW_KEY_GRAVE_ACCENT)) {

            ImGui::Begin("Key Bindings");

            ImGui::TextWrapped("E: Toggle Cursor");
            ImGui::TextWrapped("T: Toggle Flashlight");
            ImGui::TextWrapped("G: Toggle Skybox/ENV");
            ImGui::TextWrapped("X: Rotate180");
            ImGui::TextWrapped("P: Toggle Rendering Method");
            ImGui::TextWrapped("L: Sunlight");
            ImGui::TextWrapped("B: Skybox/ENV Light");
            ImGui::TextWrapped("H: Toggle SSAO");
            ImGui::TextWrapped("Y: Toggle Wireframe");

            ImGui::End();
        }

        // Rendering //

        scene2->renderDirectShadow();
        scene2->renderPointShadow();

        // Scene
        if (!deferredRender) {
            
            glViewport(0, 0, WIN_W, WIN_H);
            glBindFramebuffer(GL_FRAMEBUFFER, hdrFrame->getFBO());

            Renderer::clear();
            scene2->render();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            hdrFrame->render();

            Renderer::copyDepth(hdrFrame);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // depthFrame.render();
            
            scene2->renderLight();
            scene2->renderParticles(hdrFrame->getDepthTexture());
        }
        else {

            // ************************************************************* //

            // Geometry Pass
            glViewport(0, 0, WIN_W, WIN_H);
            deferredFrame->bindFBO();

            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer::clear();

            // mainScene->renderGbuffer();
            scene2->renderGbuffer();
            
            // ************************************************************* //

            if (SSAO::enable) {

                // Occlusion Pass
                SSAO::bindFBO();
                glClear(GL_COLOR_BUFFER_BIT);

                const Shader& shader = ShaderManager::getFrame(gfx::shader::SSAO);

                shader.use();

                shader.setInt("gPosition", 0);
                deferredFrame->bind_gPosition(0);

                shader.setInt("gNormal", 1);
                deferredFrame->bind_gNormal(1);

                SSAO::bindNoiseTex(2);
                shader.setMat4("projection", Camera::activeCamera->getProjection());
                shader.setMat4("view",       Camera::activeCamera->getView());

                frameBuffers::renderScreen();
                SSAO::blurSSAO();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ************************************************************* //

            // Lighting Pass
            scene2->renderDeferred();
            deferredFrame->render();

            Renderer::copyDepth(deferredFrame);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            scene2->renderLight();
            scene2->renderParticles(deferredFrame->getDepthTexture());
        }

        Crosshair::render();

        // Debug - UI
        DebugMenu::endUI();

        Text::render(
            "FPS: " + Text::floatToString(fps),
            glm::vec2(10.0f, 700.0f),
            0.3f,
            colors::WHITE
        );

        Text::render(
            "frametime: " + Text::floatToString(frameTime) + "ms",
            glm::vec2(10.0f, 680.0f), 
            0.3f,
            colors::WHITE
        );

        glfwSwapBuffers(window);
    }

    // ---------- Termination ----------------- //

    // mainScene->destroy();

    DebugMenu::instance().destroy();
    Renderer::instance().terminate();

    glfwTerminate();

    std::cout << "\n//----------------------------------------//" << std::endl; 

    return 0;
}
