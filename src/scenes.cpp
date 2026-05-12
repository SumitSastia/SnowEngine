#include <scenes.h>
#include <camera.h>
#include <renderer.h>

enum scene1_shaders {

    TEXTURED_CUBE,
    TEXTURED_PLANE,
    INSTANCED_CUBES,
    LIGHT,
    POINT_SHADOW,
    DIRECT_SHADOW
};

namespace scene_var {
    const float speed = 3.0f;
};

void Scene1::input(GLFWwindow* window, const float& delta_time) {

    static uint8_t model_counter = 0;
    const  float   move_speed    = scene_var::speed * delta_time;

    // Toggle Keys
    static bool KEP_1_PRESSED = false;


    // Model of the Object to move
    glm::mat4& movableModel = entityModels[model_counter];

    if (glfwGetKey(window, GLFW_KEY_KP_8)) {
        movableModel = glm::translate(movableModel, move_speed * glm::vec3( 0.0f, 0.0f,-1.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_2)) {
        movableModel = glm::translate(movableModel, move_speed * glm::vec3( 0.0f, 0.0f, 1.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_4)) {
        movableModel = glm::translate(movableModel, move_speed * glm::vec3(-1.0f, 0.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_6)) {
        movableModel = glm::translate(movableModel, move_speed * glm::vec3( 1.0f, 0.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_9)) {
        movableModel = glm::translate(movableModel, move_speed * glm::vec3( 0.0f, 1.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_7)) {
        movableModel = glm::translate(movableModel, move_speed * glm::vec3( 0.0f,-1.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_1)) {

        if (!KEP_1_PRESSED) {
            model_counter = (model_counter + 1) % entityModels.size();
        }
        KEP_1_PRESSED = true;
    }
    else KEP_1_PRESSED = false;

    if (glfwGetKey(window, GLFW_KEY_KP_5)) {

        const float rotation_speed = 2.0f;
        entityModels[2] = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_speed), glm::vec3(0.0f, 1.0f, 0.0f)) * entityModels[2];
    }

    if (glfwGetKey(window, GLFW_KEY_V)) {
        Camera::instance().set_position(glm::vec3(1.0f));
        Camera::instance().set_target(glm::vec3(-0.1f, -0.1f, -0.1f));
    }
}

void Scene1::init() {

    total_shaders  = 5;
    total_entities = 3;

    loaded_shaders = {

        Shader(
            "../shaders/normalCube/cube.vert",
            "../shaders/normalCube/texturedCube.frag", true
        ),
        Shader(
            "../shaders/planes/plane.vert",
            "../shaders/planes/texturedPlane.frag", true
        ),
        Shader(
            "../shaders/instancedCubes/texture.vert",
            "../shaders/instancedCubes/texture.frag"
        ),
        Shader(
            "../shaders/lights/light.vert",
            "../shaders/lights/light.frag"
        ),
        Shader(
            "../shaders/pointShadow/shadow.vert",
            "../shaders/pointShadow/shadow.geom",
            "../shaders/pointShadow/shadow.frag"
        ),
        Shader(
            "../shaders/directShadow/directShadow.vert",
            "../shaders/directShadow/directShadow.frag"
        )
    };

    // Entities

    myCube  = DefaultShapes::instance().cube;
    myFloor = DefaultShapes::instance().square;
    cubes   = DefaultShapes::instance().cubeInstanced;

    entities = {
        myCube
    };

    entityModels = {

        glm::mat4(1.0f),
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -3.0f)), glm::vec3(5.0f))
    };

    // Lights
    light_count = 1;

    for (uint8_t i = 0; i < light_count; i++) {

        lights.push_back(new LightSource());
        shadowFrames.push_back(new PointShadowFrame());
    }

    lights[0]->setLightColor(colors::YELLOW);
    lights[0]->setPosition(glm::vec3(3.0f, 1.5f, 2.0f));
    entityModels.push_back(lights[0]->getModel());

    // Shadow Mapping
    float near = 1.0f;
    float far  = 25.0f;

    shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);
    directFrame = new DirectShadowFrame();

    // Directional Light
    float near_plane = 1.0f, far_plane = 10.0f, size = 10.0f;
    glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, near_plane, far_plane);

    glm::mat4 lightView = glm::lookAt(
        glm::vec3(2.0f, 4.0f, 1.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    lightSpace = lightProjection * lightView;

    debugFrame = new DebugFrame(WIN_W, WIN_H);
}

void Scene1::update(const float& delta_time) {

    lights[0]->setPosition(entityModels[2][3]);
}

void Scene1::renderDebug() const {

    debugFrame->render(directFrame->getTex());
}

void Scene1::render() const {

    unsigned int loadedTextures = 0;

    const glm::mat4 projection = Camera::instance().getPerspective();
    const glm::mat4 view       = Camera::instance().getView();

    Renderer::enableCulling();

    // Light & Shadow
    Shader currentShader = loaded_shaders[LIGHT];
    currentShader.use();

    for (uint8_t i = 0; i < light_count; i++) {
        
        currentShader.setMat4("finalMatrix", projection * view * glm::scale(entityModels[2], glm::vec3(0.5f)));
        currentShader.setVec3("lightColor" , lights[i]->getLightColor());
        lights[i]->draw();

        glActiveTexture(GL_TEXTURE0 + loadedTextures++);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowFrames[i]->getTex());
    }

    const unsigned int dl_depthMap = loadedTextures++;

    glActiveTexture(GL_TEXTURE0 + dl_depthMap);
    glBindTexture(GL_TEXTURE_2D, directFrame->getTex());

    // Cube
    currentShader = loaded_shaders[TEXTURED_CUBE];
    currentShader.use();

    currentShader.setMat4("projection", projection);
    currentShader.setMat4("view"      , view);
    currentShader.setMat4("model"     , entityModels[0]);

    currentShader.setVec3("camPos"     , Camera::instance().getPos());
    currentShader.setInt ("light_count", light_count);
    currentShader.setFloat("far_plane" , 25.0f);

    for (uint8_t i = 0; i < light_count; i++) {

        currentShader.setPointLight(("pl[" + std::to_string(i) + "]"), lights[i]->getPointLight());
        currentShader.setInt(("depthMap[" + std::to_string(i) + "]").c_str(), i);
    }

    currentShader.setInt("texture0", loadedTextures);
    myCube.bindTexture(GL_TEXTURE0 + loadedTextures++);
    myCube.draw();

    Renderer::disableCulling();

    // Floor
    currentShader = loaded_shaders[TEXTURED_PLANE];
    currentShader.use();

    currentShader.setMat4("projection", projection);
    currentShader.setMat4("view"      , view);
    currentShader.setMat4("model"     , entityModels[1]);
    currentShader.setMat4("lightSpace", lightSpace);
    
    currentShader.setVec3("vNormal"    , glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
    currentShader.setVec3("camPos"     , Camera::instance().getPos());
    currentShader.setInt ("light_count", light_count);
    currentShader.setFloat("far_plane" , 25.0f);

    for (uint8_t i = 0; i < light_count; i++) {

        currentShader.setPointLight(("pl[" + std::to_string(i) + "]"), lights[i]->getPointLight());
        currentShader.setInt(("depthMap[" + std::to_string(i) + "]").c_str(), i);
    }

    currentShader.setDirectionalLight("dl", DefaultLights::instance().sunlight);
    currentShader.setInt("dl_depthMap", dl_depthMap);

    currentShader.setInt("texture0", loadedTextures);
    myFloor.bindTexture(GL_TEXTURE0 + loadedTextures++);
    myFloor.draw();
}

void Scene1::renderDirectShadow() const {

    Renderer::disableCulling();
    const Shader currentShader = loaded_shaders[DIRECT_SHADOW];
    
    glBindFramebuffer(GL_FRAMEBUFFER, directFrame->getFBO());
    glViewport(0,0, frameBuffers::shadowSize, frameBuffers::shadowSize);

    glClear(GL_DEPTH_BUFFER_BIT);

    currentShader.use();
    currentShader.setMat4("lightSpace", lightSpace);

    // Entities to cast their Shadow

    currentShader.setMat4("model", entityModels[0]);
    myCube.draw();

    // -----------------------------

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Renderer::enableCulling();
}

void Scene1::renderPointShadow() const {

    const Shader currentShader = loaded_shaders[POINT_SHADOW];

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::disableCulling();

    for (uint8_t i = 0; i < light_count; i++) {

        const unsigned int currentFBO = shadowFrames[i]->getFBO();
        const glm::vec3    lightPos   = lights[i]->getPosition();

        const std::vector <glm::mat4> shadowMatrices = {

            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f,0.0f), glm::vec3(0.0f,0.0f, 1.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f,0.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,-1.0f,0.0f))
        };

        glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
        glViewport(0, 0, frameBuffers::shadowSize, frameBuffers::shadowSize);
        glClear(GL_DEPTH_BUFFER_BIT);

        currentShader.use();

        for (uint8_t i = 0; i < 6; i++) {
            currentShader.setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowMatrices[i]);
        }

        currentShader.setVec3 ("lightPos" , lightPos);
        currentShader.setFloat("far_plane", 25.0f);

        // Entities to cast their Shadow

        currentShader.setMat4("model", entityModels[0]);
        myCube.draw();

        // -----------------------------
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene1::destroy() {

    // Cleans loaded-shaders from the Memory

    for (uint8_t i = 0; i < total_shaders; i++) {
        loaded_shaders[i].destroy();
    }
}