#include <input.h>
#include <scenes.h>
#include <camera.h>
#include <renderer.h>

#include <thread>

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
    CUBEMAP,
    PARALLAX_PLANE,
    SPECULAR_CUBE,
    INSTANCED_SPEC,
    GBUFFER_3D,
    GBUFFER_2D,
    GBUFFER_INST,
    GBUFFER_NORM3D,
    GBUFFER_NORM2D,
    PBR_3D,
    PBR_2D,
    PBR_INST,
    PBR_NORM3D,
    PBR_NORM2D
};

namespace scene_var {
    const float speed = 3.0f;
};

void Scene1::input(GLFWwindow* window, const float& delta_time) {

    static uint8_t model_counter = 0;
    const  float   move_speed    = scene_var::speed * delta_time;

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

    if (Input::isKeyDown(GLFW_KEY_KP_1)) {
        model_counter = (model_counter + 1) % entityModels.size();
    }

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

    if (Input::isKeyDown(GLFW_KEY_T)) {
        DefaultLights::instance().flashlight.isVisible = !DefaultLights::instance().flashlight.isVisible;
    }

    if (Input::isKeyDown(GLFW_KEY_G)) {
        _skybox->setVisibility(!_skybox->getVisibility());
    }
}

void Scene1::init() {

    total_shaders  = 6;
    total_entities = 3;

    loaded_shaders = {

        Shader(
            "../shaders/normalCube/cube.vert",
            "../shaders/commonFrag/texturedObj.frag", true
        ),
        Shader(
            "../shaders/planes/plane.vert",
            "../shaders/commonFrag/texturedObj.frag", true
        ),
        Shader(
            "../shaders/instancedCubes/texture.vert",
            "../shaders/commonFrag/texturedObj.frag", true
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
            "../shaders/commonFrag/normalTex.frag", true
        ),
        Shader(
            "../shaders/normalCube/normalTexCube.vert",
            "../shaders/commonFrag/normalTex.frag", true
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
        ),
        Shader(
            "../shaders/planes/parallaxPlane.vert",
            "../shaders/planes/parallaxPlane.frag", true
        ),
        Shader(
            "../shaders/normalCube/cube.vert",
            "../shaders/commonFrag/specularTex.frag", true
        ),
        Shader(
            "../shaders/instancedCubes/texture.vert",
            "../shaders/commonFrag/specularTex.frag", true
        ),
        Shader(
            "../shaders/deferred/gbuffer3d.vert",
            "../shaders/deferred/gbuffer.frag"
        ),
        Shader(
            "../shaders/deferred/gbuffer2d.vert",
            "../shaders/deferred/gbuffer.frag"
        ),
        Shader(
            "../shaders/deferred/gbufferInst.vert",
            "../shaders/deferred/gbuffer.frag"
        ),
        Shader(
            "../shaders/deferred/gbuffer_normalTex3d.vert",
            "../shaders/deferred/gbuffer_normalTex.frag"
        ),
        Shader(
            "../shaders/deferred/gbuffer_normalTex2d.vert",
            "../shaders/deferred/gbuffer_normalTex.frag"
        ),
        Shader(
            "../shaders/normalCube/cube.vert",
            "../shaders/commonFrag/pbr.frag", true
        ),
        Shader(
            "../shaders/planes/plane.vert",
            "../shaders/commonFrag/pbr.frag", true
        ),
        Shader(
            "../shaders/instancedCubes/texture.vert",
            "../shaders/commonFrag/pbr.frag", true
        ),
        Shader(
            "../shaders/normalCube/normalTexCube.vert",
            "../shaders/commonFrag/pbr_normalTex.frag", true
        ),
        Shader(
            "../shaders/planes/normalTexPlane.vert",
            "../shaders/commonFrag/pbr_normalTex.frag", true
        )
    };

    // Entities

    myCube   = DefaultShapes::instance().cube;
    myFloor  = DefaultShapes::instance().square;
    cubes    = DefaultShapes::instance().cubeInstanced;
    myGround = DefaultShapes::instance().square;
    advCube  = DefaultShapes::instance().advancedCube;
    myWall   = DefaultShapes::instance().square.copy();

    mySphere = new Model3D("../assets/models/test_cube/sphere.obj");

    // cubes.loadDiffuseTex("assets/textures/pure_red.png");

    // myFloor.loadNormalTex("assets/textures/texture_maps/wall_normal.png");

    myWall.loadDiffuseTex("assets/textures/parallex_maps/bricks2.jpg");
    myWall.loadNormalTex("assets/textures/parallex_maps/bricks2_normal.jpg");
    myWall.loadSpecularTex("assets/textures/parallex_maps/bricks2_disp.jpg");

    // loaded_entities.push_back(myCube);
    // loaded_entities.push_back(myFloor);
    // loaded_entities.push_back(myGround);
    // loaded_entities.push_back(advCube);
    // loaded_entities.push_back(*mySphere);

    Matrix4 cubeModel   {};
    Matrix4 floorModel  {};
    Matrix4 groundModel {};
    Matrix4 sphereModel {};
    Matrix4 wallModel   {};

    floorModel.translate(glm::vec3(-2.5f, 0.5f, -5.0f));
    floorModel.scale(glm::vec3(5.0f));

    groundModel.translate(glm::vec3(0.0f, -2.0f, 0.0f));
    groundModel.rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    groundModel.scale(glm::vec3(10.0f));

    sphereModel.translate(glm::vec3(6.0, 0.0, 3.0));

    wallModel.translate(glm::vec3(2.5f, 0.5f, -5.0f));
    wallModel.scale(glm::vec3(2.0f));

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

    lights[0]->setLightColor(2.0f * colors::YELLOW);
    lights[0]->setPosition(lightModel0.getPos());

    lights[1]->setLightColor(2.0f * colors::PINK);
    lights[1]->setPosition(lightModel1.getPos());

    entityModels.push_back(cubeModel);    // 0
    entityModels.push_back(floorModel);   // 1
    entityModels.push_back(lightModel0);  // 2
    entityModels.push_back(groundModel);  // 3
    entityModels.push_back(sphereModel);  // 4
    entityModels.push_back(lightModel1);  // 5
    entityModels.push_back(wallModel);    // 6

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
    
    // Skybox

    std::vector <std::string> skyboxFaces = {

        "assets/textures/skybox/Daylight_Box_Right.bmp",
        "assets/textures/skybox/Daylight_Box_Left.bmp",
        "assets/textures/skybox/Daylight_Box_Top.bmp",
        "assets/textures/skybox/Daylight_Box_Bottom.bmp",
        "assets/textures/skybox/Daylight_Box_Front.bmp",
        "assets/textures/skybox/Daylight_Box_Back.bmp"
    };

    std::vector <std::string> envFaces = {
        
        "assets/textures/skybox/Daylight_Box_Right_irradiance.png",
        "assets/textures/skybox/Daylight_Box_Left_irradiance.png",
        "assets/textures/skybox/Daylight_Box_Top_irradiance.png",
        "assets/textures/skybox/Daylight_Box_Bottom_irradiance.png",
        "assets/textures/skybox/Daylight_Box_Front_irradiance.png",
        "assets/textures/skybox/Daylight_Box_Back_irradiance.png"
    };
    
    _skybox = new Skybox(skyboxFaces);
    // _skybox->setIrradianceMap(envFaces);

    // ECS
    cube = ecs.createEntity();

    ibl_frame = new IBLFrame("assets/env/hdri-sky.hdr", 1024);

    // Debug
    debugFrame = new DebugFrame(WIN_W, WIN_H);

    std::cout << "Scene1 - Loaded Shaders: "  << loaded_shaders.size() << std::endl;
    // std::cout << "Scene1 - Loaded Entities: " << loaded_entities.size() + cubes.getCount() << std::endl;
}

void Scene1::update(const float& delta_time) {

    for (uint8_t i = 0; i < light_count; i++) {
        lights[i]->setPosition(lightModels[i]->getPos());
    }

    DefaultLights::instance().update();
}

void Scene1::renderDebug() const {

    // debugFrame->render(directFrame->getTex());
}

void Scene1::render() const {

    unsigned int loadedTextures = 0;

    const glm::mat4 projection = Camera::instance().getPerspective();
    const glm::mat4 view       = Camera::instance().getView();

    Renderer::enableCulling();

    // Light & Shadow
    Shader currentShader = loaded_shaders[LIGHT];
    // currentShader.use();

    for (uint8_t i = 0; i < light_count; i++) {
        shadowFrames[i]->bindTexture(loadedTextures++);
    }

    const unsigned int dl_depthMap = loadedTextures++;
    directFrame->bindTexture(dl_depthMap);

    const unsigned int env_irradiance = loadedTextures++;
    // ibl_frame->bindIrradianceMap(env_irradiance);

    const unsigned int env_prefilter = loadedTextures++;
    // ibl_frame->bindPreFilterMap(env_prefilter);

    std::vector <uint8_t> commonShaders = {0,2,6,7,8,11,12,13,19,20,21,22,23};
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

        currentShader.setBool("useIrradiance", _skybox->getVisibility());
        currentShader.setInt("irradianceMap", env_irradiance);
    }

    // Cube
    // currentShader = loaded_shaders[SPECULAR_CUBE];
    // currentShader = loaded_shaders[PBR_3D];
    currentShader = loaded_shaders[PBR_NORM3D];
    currentShader.use();

    currentShader.setMat4("model",        entityModels[0].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[0].getNormal());
    
    currentShader.setInt("texture0", loadedTextures);
    advCube.bindDiffuseTex(loadedTextures++);

    // currentShader.setInt("texture1", loadedTextures);
    // advCube.bindNormalTex(loadedTextures++);

    currentShader.setInt("texture3", loadedTextures);
    advCube.bindNormalTex(loadedTextures++);

    advCube.draw();

    currentShader = loaded_shaders[PBR_3D];
    currentShader.use();

    currentShader.setMat4("model",        entityModels[4].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[4].getNormal());

    currentShader.setInt("texture0", loadedTextures);
    mySphere->bindTextures(loadedTextures++);

    currentShader.setInt("env", loadedTextures);
    // _skybox->bindTexture(loadedTextures++);
    // ibl_frame->bindEnv(loadedTextures++);

    currentShader.setInt("preFilterMap", loadedTextures);
    // ibl_frame->bindPreFilterMap(loadedTextures++);

    currentShader.setInt("brdfLUT", loadedTextures);
    // ibl_frame->bindBRDFLUT(loadedTextures++);

    mySphere->draw();

    // Instanced Cubes
    // currentShader = loaded_shaders[INSTANCED_SPEC];
    currentShader = loaded_shaders[PBR_INST];
    currentShader.use();

    // currentShader.setFloat("metallic", 0.1F);
    // currentShader.setFloat("roughness", 0.2F);

    currentShader.setInt("texture0", loadedTextures);
    cubes.bindDiffuseTex(loadedTextures++);

    // currentShader.setInt("texture2", loadedTextures);
    // cubes.bindSpecularTex(loadedTextures++);

    currentShader.setInt("env", loadedTextures);
    // _skybox->bindTexture(loadedTextures++);
    // ibl_frame->bindEnv(loadedTextures++);

    currentShader.setInt("preFilterMap", loadedTextures);
    // ibl_frame->bindPreFilterMap(loadedTextures++);

    currentShader.setInt("brdfLUT", loadedTextures);
    // ibl_frame->bindBRDFLUT(loadedTextures++);

    cubes.draw();

    Renderer::disableCulling();

    // Floor & Ground
    // currentShader = loaded_shaders[NORMAL_MAPPED_2D];
    // currentShader = loaded_shaders[PBR_2D];
    currentShader = loaded_shaders[PBR_NORM2D];
    currentShader.use();

    currentShader.setMat4("model",        entityModels[1].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[1].getNormal());

    currentShader.setInt("texture0", loadedTextures);
    myFloor.bindDiffuseTex(loadedTextures++);
    // ibl_frame->bindBRDFLUT(loadedTextures++);

    currentShader.setInt("texture3", loadedTextures);
    myFloor.bindNormalTex(loadedTextures++);

    myFloor.draw();

    currentShader.setMat4("model",        entityModels[3].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[3].getNormal());
    myGround.draw();

    // Parallax Wall
    currentShader = loaded_shaders[PARALLAX_PLANE];
    currentShader.use();

    currentShader.setVec3("lightPos", lights[0]->getPosition());
    currentShader.setVec3("viewPos", Camera::instance().getPos());

    currentShader.setMat4("model",        entityModels[6].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[6].getNormal());

    currentShader.setInt("texture0", loadedTextures);
    myWall.bindDiffuseTex(loadedTextures++);

    currentShader.setInt("texture1", loadedTextures);
    myWall.bindNormalTex(loadedTextures++);

    currentShader.setInt("texture2", loadedTextures);
    myWall.bindSpecularTex(loadedTextures++);

    currentShader.setFloat("height_scale", 0.1f);

    myWall.draw();

    // ECS
    RenderSystem::instance().render(EntityShapes::instance().getECS());

    // Testing ENV

    glDepthFunc(GL_LEQUAL);
    Renderer::disableCulling();

    currentShader = loaded_shaders[CUBEMAP];
    currentShader.use();

    currentShader.setMat4("projection", projection);
    currentShader.setMat4("view"      , glm::mat4(glm::mat3(view)));

    currentShader.setInt("cubeMap", 0);
    ibl_frame->bindEnv(0);
    // ibl_frame->bindPreFilterMap(0);

    if (_skybox->getVisibility()) ibl_frame->draw(0);

    glDepthFunc(GL_LESS);
}

void Scene1::renderGbuffer() const {

    unsigned int loadedTextures = 0;

    const glm::mat4 projection = Camera::instance().getPerspective();
    const glm::mat4 view       = Camera::instance().getView();

    std::vector <uint8_t> commonShaders = {14,15,16,17,18};
    const uint8_t _length = commonShaders.size();

    Shader currentShader = loaded_shaders[GBUFFER_3D];

    for (uint8_t i = 0; i < _length; i++) {

        currentShader = loaded_shaders[commonShaders[i]];
        currentShader.use();

        currentShader.setMat4("projection", projection);
        currentShader.setMat4("view"      , view);
    }

    Renderer::disableCulling();

    // Cube
    currentShader = loaded_shaders[GBUFFER_3D];
    currentShader.use();

    // currentShader.setMat4("model",        entityModels[0].getMatrix());
    // currentShader.setMat3("normalMatrix", entityModels[0].getNormal());
    
    // currentShader.setInt("texture0", loadedTextures);
    // myCube.bindDiffuseTex(loadedTextures++);
    // myCube.draw();

    // Models
    currentShader.setMat4("model",        entityModels[4].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[4].getNormal());

    currentShader.setInt("texture0", loadedTextures);
    mySphere->bindTextures(loadedTextures++);
    mySphere->draw();

    // Normal-Mapped Cube
    currentShader = loaded_shaders[GBUFFER_NORM3D];
    currentShader.use();

    currentShader.setMat4("model",        entityModels[0].getMatrix());
    currentShader.setMat3("normalMatrix", entityModels[0].getNormal());
    
    currentShader.setInt("texture0", loadedTextures);
    advCube.bindDiffuseTex(loadedTextures++);

    currentShader.setInt("texture1", loadedTextures);
    advCube.bindNormalTex(loadedTextures++);
    advCube.draw();

    // Instanced
    currentShader = loaded_shaders[GBUFFER_INST];
    currentShader.use();

    currentShader.setInt("texture0", loadedTextures);
    cubes.bindDiffuseTex(loadedTextures++);

    cubes.draw();

    // Planes
    // currentShader = loaded_shaders[GBUFFER_2D];
    currentShader = loaded_shaders[GBUFFER_NORM2D];
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
}

void Scene1::renderDeferred(const Shader& currentShader) const {

    // Not Working
    // 0 - gPosition
    // 1 - gNormal
    // 2 - gTexture

    unsigned int loadedTextures = 0;

    currentShader.use();

    currentShader.setVec3("camPos"     , Camera::instance().getPos());
    currentShader.setInt ("light_count", light_count);
    currentShader.setFloat("far_plane" , 25.0f);

    for (uint8_t i = 0; i < light_count; i++) {

        currentShader.setPointLight(("pl[" + std::to_string(i) + "]"), lights[i]->getPointLight());
        currentShader.setInt(("depthMap[" + std::to_string(i) + "]").c_str(), loadedTextures);

        shadowFrames[i]->bindTexture(loadedTextures++);
    }

    const unsigned int dl_depthMap = loadedTextures++;
    directFrame->bindTexture(dl_depthMap);

    currentShader.setDirectionalLight("dl", DefaultLights::instance().sunlight);
    currentShader.setInt("dl_depthMap", dl_depthMap);

    currentShader.setBool("useSpotLight", DefaultLights::instance().flashlight.isVisible);
    currentShader.setSpotLight("sl", DefaultLights::instance().flashlight);

    currentShader.setFloat("skyboxIntensity", _skybox->getVisibility()? _skybox->getIntensity() : 0.0f);
}

void Scene1::renderLight() const {

    const glm::mat4 projection = Camera::instance().getPerspective();
    const glm::mat4 view       = Camera::instance().getView();

    // Light
    Shader currentShader = loaded_shaders[LIGHT];
    currentShader.use();

    for (uint8_t i = 0; i < light_count; i++) {

        currentShader.setMat4("finalMatrix", projection * view * glm::scale(lightModels[i]->getMatrix(), glm::vec3(0.5f)));
        currentShader.setVec3("lightColor" , lights[i]->getLightColor());
        lights[i]->draw();
    }

    // Skybox
    // this->renderSkybox();
}

void Scene1::renderSkybox() const {

    if (!_skybox->getVisibility()) return;

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    const Shader currentShader = loaded_shaders[CUBEMAP];
    currentShader.use();

    currentShader.setMat4("projection", Camera::instance().getPerspective());
    currentShader.setMat4("view", glm::mat4(glm::mat3(Camera::instance().getView())));

    currentShader.setInt("cubeMap", 0);
    _skybox->bindTexture(0);

    _skybox->draw();

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
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

        currentShader.setMat4("model", entityModels[3].getMatrix());
        myGround.draw();

        currentShader.setMat4("model", entityModels[6].getMatrix());
        myWall.draw();

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