#include <scenes.h>
#include <camera.h>
#include <renderer.h>

glm::vec3 customPos = glm::vec3(-3.0f, 1.5f, 3.0f);

enum scene1_shaders {

    TEXTURED_CUBE,
    TEXTURED_PLANE,
    INSTANCED_CUBES,
    LIGHT,
    POINT_SHADOW,
    DIRECT_SHADOW,
    NORMAL_MAPPED_2D,
    NORMAL_MAPPED_3D,
    MODEL_3D,
    POINT_SHADOW_INST,
    CUBEMAP
};

namespace scene_var {
    const float speed = 3.0f;
};

void Scene1::input(GLFWwindow* window, const float& delta_time) {

    static uint8_t model_counter = 0;
    const  float   move_speed    = scene_var::speed * delta_time;

    // Toggle Keys
    static bool KEP_1_PRESSED = false;
    static bool KEY_T_PRESSED = false;
    static bool KEY_G_PRESSED = false;

    // Model of the Object to move
    Matrix4& movableModel = entityModels[model_counter];

    if (glfwGetKey(window, GLFW_KEY_KP_8)) {
        movableModel.translate(move_speed * glm::vec3( 0.0f, 0.0f,-1.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_2)) {
        movableModel.translate(move_speed * glm::vec3( 0.0f, 0.0f, 1.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_4)) {
        movableModel.translate(move_speed * glm::vec3(-1.0f, 0.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_6)) {
        movableModel.translate(move_speed * glm::vec3( 1.0f, 0.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_9)) {
        movableModel.translate(move_speed * glm::vec3( 0.0f, 1.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_7)) {
        movableModel.translate(move_speed * glm::vec3( 0.0f,-1.0f, 0.0f));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_1)) {

        if (!KEP_1_PRESSED) model_counter = (model_counter + 1) % entityModels.size();
        KEP_1_PRESSED = true;
    }
    else KEP_1_PRESSED = false;

    if (glfwGetKey(window, GLFW_KEY_KP_5)) {

        const float rotation_speed = 2.0f;
        
        Matrix4 _matrix {};
        _matrix.rotate(rotation_speed, glm::vec3(0.0f, 1.0f, 0.0f));

        lightModels[0]->setMatrix(_matrix.getMatrix() * lightModels[0]->getMatrix());
        lightModels[1]->setMatrix(_matrix.getMatrix() * lightModels[1]->getMatrix());
    }

    if (glfwGetKey(window, GLFW_KEY_V)) {

        Camera::instance().set_position(glm::vec3(1.0f));
        Camera::instance().set_target(glm::vec3(-0.1f, -0.1f, -0.1f));
    }

    if (glfwGetKey(window, GLFW_KEY_T)) {

        if (!KEY_T_PRESSED) {
            DefaultLights::instance().flashlight.isVisible = !DefaultLights::instance().flashlight.isVisible;
        }
        KEY_T_PRESSED = true;
    }
    else KEY_T_PRESSED = false;

    if (glfwGetKey(window, GLFW_KEY_G)) {

        if (!KEY_G_PRESSED) _skybox->setVisibility(!_skybox->getVisibility());
        KEY_G_PRESSED = true;
    }
    else KEY_G_PRESSED = false;
}

void Scene1::init() {

    total_shaders  = 6;
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
            "../shaders/instancedCubes/texture.frag", true
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
        ),
        Shader(
            "../shaders/planes/normalTexPlane.vert",
            "../shaders/planes/normalTexPlane.frag", true
        ),
        Shader(
            "../shaders/normalCube/normalTexCube.vert",
            "../shaders/normalCube/normalTexCube.frag", true
        ),
        Shader(
            "../shaders/models/model.vert",
            "../shaders/models/texturedModel.frag", true
        ),
        Shader(
            "../shaders/pointShadow/instancedShadow.vert",
            "../shaders/pointShadow/shadow.geom",
            "../shaders/pointShadow/shadow.frag"
        ),
        Shader(
            "../shaders/cubeMap/skybox.vert",
            "../shaders/cubeMap/skybox.frag"
        )
    };

    // Entities

    myCube   = DefaultShapes::instance().cube;
    myFloor  = DefaultShapes::instance().square;
    cubes    = DefaultShapes::instance().cubeInstanced;
    myGround = DefaultShapes::instance().square;
    advCube  = DefaultShapes::instance().advancedCube;

    mySphere = new Model3D("../assets/models/test_cube/sphere.obj");

    entities = {
        myCube,
        advCube
    };

    Matrix4 cubeModel   {};
    Matrix4 floorModel  {};
    Matrix4 groundModel {};
    Matrix4 sphereModel {};

    floorModel.translate(glm::vec3(-2.0f, 0.0f, -5.0f));
    floorModel.scale(glm::vec3(5.0f));

    groundModel.translate(glm::vec3(0.0f, -2.0f, 0.0f));
    groundModel.rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    groundModel.scale(glm::vec3(10.0f));

    sphereModel.translate(glm::vec3(6.0, 0.0, 3.0));

    // Lights
    light_count = 2;

    for (uint8_t i = 0; i < light_count; i++) {

        lights.push_back(new LightSource());
        shadowFrames.push_back(new PointShadowFrame());
    }

    Matrix4 lightModel0 {};
    Matrix4 lightModel1 {};

    lightModel0.translate(glm::vec3(3.0f, 1.5f, -3.0f));
    lightModel1.translate(glm::vec3(-3.0f, 1.5f, 3.0f));

    lights[0]->setLightColor(colors::YELLOW);
    lights[0]->setPosition(lightModel0.getPos());

    lights[1]->setLightColor(colors::PINK);
    lights[1]->setPosition(lightModel1.getPos());

    entityModels.push_back(cubeModel);    // 0
    entityModels.push_back(floorModel);   // 1
    entityModels.push_back(lightModel0);  // 2
    entityModels.push_back(groundModel);  // 3
    entityModels.push_back(sphereModel);  // 4
    entityModels.push_back(lightModel1);  // 5

    lightModels.push_back(&entityModels[2]);
    lightModels.push_back(&entityModels[5]);

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

    // Debug
    debugFrame = new DebugFrame(WIN_W, WIN_H);

    // Skybox

    // Skybox
    std::vector <std::string> skyboxFaces = {
        "assets/textures/skybox/Daylight_Box_Right.bmp",
        "assets/textures/skybox/Daylight_Box_Left.bmp",
        "assets/textures/skybox/Daylight_Box_Top.bmp",
        "assets/textures/skybox/Daylight_Box_Bottom.bmp",
        "assets/textures/skybox/Daylight_Box_Front.bmp",
        "assets/textures/skybox/Daylight_Box_Back.bmp"
    };

    _skybox = new Skybox(skyboxFaces);
}

void Scene1::update(const float& delta_time) {

    for (uint8_t i = 0; i < light_count; i++) {
        lights[i]->setPosition(lightModels[i]->getPos());
    }

    DefaultLights::instance().update();
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
        
        currentShader.setMat4("finalMatrix", projection * view * glm::scale(lightModels[i]->getMatrix(), glm::vec3(0.5f)));
        currentShader.setVec3("lightColor" , lights[i]->getLightColor());
        lights[i]->draw();

        glActiveTexture(GL_TEXTURE0 + loadedTextures++);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowFrames[i]->getTex());
    }

    const unsigned int dl_depthMap = loadedTextures++;

    glActiveTexture(GL_TEXTURE0 + dl_depthMap);
    glBindTexture(GL_TEXTURE_2D, directFrame->getTex());

    std::vector <uint8_t> commonShaders = {2,6,7,8};
    const uint8_t _length = commonShaders.size();

    const bool showSkybox = _skybox->getVisibility();

    // Setting common Shader Settings
    for (uint8_t i = 0; i < _length; i++) {

        currentShader = loaded_shaders[commonShaders[i]];
        currentShader.use();

        currentShader.setMat4("projection", projection);
        currentShader.setMat4("view"      , view);
        currentShader.setMat4("lightSpace", lightSpace);

        currentShader.setVec3("camPos"     , Camera::instance().getPos());
        currentShader.setInt ("light_count", light_count);
        currentShader.setFloat("far_plane" , 25.0f);

        for (uint8_t j = 0; j < light_count; j++) {

            currentShader.setPointLight(("pl[" + std::to_string(j) + "]"), lights[j]->getPointLight());
            currentShader.setInt(("depthMap[" + std::to_string(j) + "]").c_str(), j);
        }

        currentShader.setDirectionalLight("dl", DefaultLights::instance().sunlight);
        currentShader.setInt("dl_depthMap", dl_depthMap);

        currentShader.setBool("useSpotLight", DefaultLights::instance().flashlight.isVisible);
        currentShader.setSpotLight("sl", DefaultLights::instance().flashlight);

        currentShader.setFloat("skyboxIntensity", showSkybox? _skybox->getIntensity() : 0.0f);
    }

    // Cube
    currentShader = loaded_shaders[NORMAL_MAPPED_3D];
    currentShader.use();

    currentShader.setMat4("model",        entityModels[0].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[0].getNormal());
    
    currentShader.setInt("texture0", loadedTextures);
    advCube.bindDiffuseTex(loadedTextures++);

    currentShader.setInt("texture1", loadedTextures);
    advCube.bindNormalTex(loadedTextures++);
    advCube.draw();

    // Instanced Cubes
    currentShader = loaded_shaders[INSTANCED_CUBES];
    currentShader.use();

    currentShader.setInt("texture0", loadedTextures);
    cubes.bindDiffuseTex(loadedTextures++);

    cubes.draw();

    Renderer::disableCulling();

    // Floor & Ground
    currentShader = loaded_shaders[NORMAL_MAPPED_2D];
    currentShader.use();

    currentShader.setMat4("model",        entityModels[1].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[1].getNormal());

    currentShader.setInt("texture0", loadedTextures);
    myFloor.bindDiffuseTex(loadedTextures++);

    currentShader.setInt("texture1", loadedTextures);
    myFloor.bindNormalTex(loadedTextures++);
    myFloor.draw();

    currentShader.setMat4("model",        entityModels[3].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[3].getNormal());
    myGround.draw();

    // Models
    currentShader = loaded_shaders[MODEL_3D];
    currentShader.use();

    currentShader.setMat4("model",        entityModels[4].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[4].getNormal());

    currentShader.setInt("texture0", loadedTextures);
    mySphere->bindTextures(loadedTextures++);
    mySphere->draw();

    // Skybox
    if (showSkybox) this->renderSkybox(loadedTextures);
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

    currentShader.setMat4("model", entityModels[0].getMatrix());
    myCube.draw();

    // -----------------------------

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Renderer::enableCulling();
}

void Scene1::renderPointShadow() const {

    Shader currentShader = loaded_shaders[POINT_SHADOW];

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

        currentShader = loaded_shaders[POINT_SHADOW];
        currentShader.use();

        for (uint8_t j = 0; j < 6; j++) {
            currentShader.setMat4(("shadowMatrices[" + std::to_string(j) + "]").c_str(), shadowMatrices[j]);
        }

        currentShader.setVec3 ("lightPos" , lightPos);
        currentShader.setFloat("far_plane", 25.0f);

        // Entities to cast their Shadow

        currentShader.setMat4("model", entityModels[0].getMatrix());
        myCube.draw();

        currentShader.setMat4("model", entityModels[1].getMatrix());
        myFloor.draw();

        currentShader.setMat4("model", entityModels[4].getMatrix());
        mySphere->draw();

        // For instanced objects

        currentShader = loaded_shaders[POINT_SHADOW_INST];
        currentShader.use();

        for (uint8_t j = 0; j < 6; j++) {
            currentShader.setMat4(("shadowMatrices[" + std::to_string(j) + "]").c_str(), shadowMatrices[j]);
        }

        currentShader.setVec3 ("lightPos" , lightPos);
        currentShader.setFloat("far_plane", 25.0f);

        cubes.draw();

        // -----------------------------
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene1::renderSkybox(const unsigned int loadedTextures) const {

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    const Shader currentShader = loaded_shaders[CUBEMAP];
    currentShader.use();

    currentShader.setMat4("projection", Camera::instance().getPerspective());
    currentShader.setMat4("view", glm::mat4(glm::mat3(Camera::instance().getView())));

    currentShader.setInt("cubeMap", loadedTextures);
    _skybox->bindTexture(loadedTextures);

    _skybox->draw();

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

void Scene1::destroy() {

    // Cleans loaded-shaders from the Memory

    for (uint8_t i = 0; i < total_shaders; i++) {
        loaded_shaders[i].destroy();
    }

    for (uint8_t i = 0; i < light_count; i++) {

        // lights[i]->destroy();
        shadowFrames[i]->destroy();

        delete shadowFrames[i];
    }

    _skybox->destroy();
    mySphere->destroy();

    delete mySphere;
}