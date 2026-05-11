#include <scenes.h>
#include <camera.h>
#include <renderer.h>

enum scene1_shaders {

    TEXTURED_CUBE,
    TEXTURED_PLANE,
    INSTANCED_CUBES,
    LIGHT,
    POINTSHADOW
};

namespace scene_var {
    const uint8_t speed = 3;
};

void Scene1::input(GLFWwindow* window, const float& delta_time) {

    if (glfwGetKey(window, GLFW_KEY_KP_8)) {
        lights[0]->setZ(-1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_5)) {
        lights[0]->setZ(1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_4)) {
        lights[0]->setX(-1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_6)) {
        lights[0]->setX(1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_9)) {
        lights[0]->setY(1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_7)) {
        lights[0]->setY(-1 * delta_time * scene_var::speed);
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

    float near = 1.0f;
    float far  = 25.0f;

    shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);

    lights[0]->setLightColor(colors::YELLOW);
    lights[0]->setPosition(glm::vec3(3.0f, 1.5f, 2.0f));
}

void Scene1::render() const {

    unsigned int loadedTextures = 0;

    const glm::mat4 projection = Camera::instance().getPerspective();
    const glm::mat4 view       = Camera::instance().getView();

    Renderer::enableCulling();

    // Light
    Shader currentShader = loaded_shaders[LIGHT];
    currentShader.use();

    for (uint8_t i = 0; i < light_count; i++) {
    
        glm::mat4 lightModel = lights[i]->getModel();
        lightModel = glm::scale(lightModel, glm::vec3(0.5));
        
        currentShader.setMat4("finalMatrix", projection * view * lightModel);
        currentShader.setVec3("lightColor" , lights[i]->getLightColor());
        lights[i]->draw();

        glActiveTexture(GL_TEXTURE0 + loadedTextures++);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowFrames[i]->getTex());
    }

    // Cube
    currentShader = loaded_shaders[TEXTURED_CUBE];
    currentShader.use();

    currentShader.setMat4("projection", projection);
    currentShader.setMat4("view"      , view);
    currentShader.setMat4("model"     , entityModels[0]);

    currentShader.setVec3("camPos"     , Camera::instance().getPos());
    currentShader.setInt ("light_count", light_count);
    currentShader.setPointLight("pl[0]", lights[0]->getPointLight());
    
    currentShader.setFloat("far_plane"   , 25.0f);
    currentShader.setInt  ("depthMap[0]" , 0);

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

    currentShader.setVec3("vNormal"    , glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
    currentShader.setVec3("camPos"     , Camera::instance().getPos());
    currentShader.setInt ("light_count", light_count);
    currentShader.setPointLight("pl[0]", lights[0]->getPointLight());

    currentShader.setFloat("far_plane"   , 25.0f);
    currentShader.setInt  ("depthMap[0]" , 0);

    currentShader.setInt("texture0", loadedTextures);
    myFloor.bindTexture(GL_TEXTURE0 + loadedTextures++);
    myFloor.draw();
}

void Scene1::renderShadow() const {

    const Shader currentShader = loaded_shaders[POINTSHADOW];

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