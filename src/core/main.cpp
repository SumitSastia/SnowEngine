#include <iostream>

#include <renderer.h>
#include <shader.h>
#include <camera.h>
#include <shapes.h>
#include <debug.h>
#include <input.h>
#include <scenes.h>
#include <model.h>
#include <text.h>
#include <ssao.h>

// ------------------------------ Global Variables ----------------------------------- //

bool isRunning = true;

float deltaTime = 0.0f;
float lastTime  = 0.0f;

// ----------------------------------------------------------------------------------- //

int main() {

    // ---------- Initialization --------------- //

    running::globalTimer::start();
    std::cout << "//----------------------------------------//\n\n"; 
    
    GLFWwindow* window = Renderer::instance().getWindow();

    // Hides the cursor at the Startup
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Input::init();
    DebugMenu::init(window);
    Text::init("../assets/fonts/BlockBlueprint.ttf");
    SSAO::init();
    // ShadowSystem::init();
    Shaders::initShaders();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glDisable(GL_FRAMEBUFFER_SRGB);

    // glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---------- CallBack Functions ---------- //

    glfwSetKeyCallback(window, Input::key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ---------- Testing --------------------- //

    Camera& mainCamera = Camera::instance();

    running::core::timer t;

    // Scene1* scene1 = new Scene1();
    // Scene* mainScene = scene1;
    DebugMenu::log("Time taken to initialize - scene1: " + t.end());

    // running::time::startInterval();
    Scene2* scene2   = new Scene2();
    // Scene* mainScene = scene2;
    // std::cout << "Time taken to initialize - scene2: " << running::time::endInterval() << std::endl;

    DebugFrame* debugFrame = new DebugFrame(WIN_W, WIN_H);
    HDRFrame*   hdrFrame   = new HDRFrame(WIN_W, WIN_H);
    // BloomFrame* bloomFrame = new BloomFrame(WIN_W, WIN_H);

    Gbuffer* deferredFrame = new Gbuffer(WIN_W, WIN_H);

    bool deferredRender = false;

    DebugMenu::log("Time taken to Initialize: " + running::globalTimer::getTime());

    // ---------- Loop ------------------------ //

    while (!glfwWindowShouldClose(window) && isRunning) {

        // Time
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        DebugMenu::beginUI();

        // Inputs //
        if (Input::isKeyDown(GLFW_KEY_H)) {
            SSAO::enable = !SSAO::enable;
        }

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

        DefaultLights::instance().update();
        mainCamera.input_handler(window, deltaTime);
        // mainScene->input(window, deltaTime);
        scene2->input(window, deltaTime);
        
        mainCamera.update(deltaTime);
        // mainScene->update(deltaTime);

        ImGui::End();

        // Rendering //

        scene2->renderDirectShadow();
        scene2->renderPointShadow();


        // Scene
        if (!deferredRender) {

            // mainScene->renderDirectShadow();
            // mainScene->renderPointShadow();
            
            glViewport(0, 0, WIN_W, WIN_H);
            glBindFramebuffer(GL_FRAMEBUFFER, hdrFrame->getFBO());

            Renderer::clear();

            // if (Input::isKeyPressed(GLFW_KEY_O))
            //     mainScene->render();
            
            // Renderer::enableDepth();
            // Renderer::disableCulling();
            scene2->render();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            hdrFrame->render();

            Renderer::copyDepth(hdrFrame);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // if (Input::isKeyPressed(GLFW_KEY_O))
            //     mainScene->renderLight();
            
            scene2->renderLight();
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
                
                SSAO::shader->use();

                SSAO::shader->setInt("gPosition", 0);
                deferredFrame->bind_gPosition(0);

                SSAO::shader->setInt("gNormal", 1);
                deferredFrame->bind_gNormal(1);

                SSAO::bindNoiseTex(2);
                SSAO::shader->setMat4("projection", mainCamera.getPerspective());
                SSAO::shader->setMat4("view", mainCamera.getView());

                frameBuffers::renderScreen();
                SSAO::blurSSAO();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ************************************************************* //

            // Lighting Pass
            // mainScene->renderDeferred(*deferredFrame->getShader());
            scene2->renderDeferred(*deferredFrame->getShader());
            deferredFrame->render();

            Renderer::copyDepth(deferredFrame);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // mainScene->renderLight();
            scene2->renderLight();
        }

        // Debug - UI
        DebugMenu::endUI();

        // Text::render((deferredRender)? "Deferred Rendering" : "Forward Rendering", glm::vec2(50.0f), 1.0, glm::vec3(1.0f));

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
