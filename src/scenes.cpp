#include <scenes.h>
#include <camera.h>
#include <renderer.h>

namespace scene_var {
    const u_int8_t speed = 3;
};

void Scene1::input(GLFWwindow* window, const float& delta_time) {

    if (glfwGetKey(window, GLFW_KEY_KP_8)) {
        light1.setZ(-1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_5)) {
        light1.setZ(1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_4)) {
        light1.setX(-1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_6)) {
        light1.setX(1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_9)) {
        light1.setY(1 * delta_time * scene_var::speed);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_7)) {
        light1.setY(-1 * delta_time * scene_var::speed);
    }
}

void Scene1::init() {

    total_shaders  = 3;
    total_entities = 3;

    loaded_shaders = {

        Shader(
            "../shaders/normalCube/cube.vert",
            "../shaders/normalCube/texturedCube.frag"
        ),
        Shader(
            "../shaders/planes/plane.vert",
            "../shaders/planes/texturedPlane.frag"
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

    light1.setLightColor(colors::YELLOW);
    light1.setPosition(glm::vec3(3.0f, 1.5f, 2.0f));

    entityModels = {

        glm::mat4(1.0f),
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -3.0f)), glm::vec3(5.0f))
    };
}

void Scene1::render() const {

    const glm::mat4 projection = Camera::instance().getPerspective();
    const glm::mat4 view       = Camera::instance().getView();

    Renderer::enableCulling();

    // Cube
    Shader currentShader = loaded_shaders[0];
    currentShader.use();

    currentShader.setMat4("projection", projection);
    currentShader.setMat4("view"      , view);
    currentShader.setMat4("model"     , glm::mat4(1.0f));

    currentShader.setVec3("camPos"     , Camera::instance().getPos());
    currentShader.setInt ("light_count", 1);
    currentShader.setPointLight("pl[0]", light1.getPointLight());

    myCube.bindTexture(GL_TEXTURE0);
    currentShader.setInt("texture0", 0);
    myCube.draw();
    
    // Light
    currentShader = loaded_shaders[3];
    currentShader.use();
    
    glm::mat4 lightModel = light1.getModel();
    lightModel = glm::scale(lightModel, glm::vec3(0.5));
    
    currentShader.setMat4("finalMatrix", projection * view * lightModel);
    currentShader.setVec3("lightColor", light1.getLightColor());
    light1.draw();

    Renderer::disableCulling();

    // Floor
    currentShader = loaded_shaders[1];
    currentShader.use();

    currentShader.setMat4("projection", projection);
    currentShader.setMat4("view"      , view);
    currentShader.setMat4("model"     , entityModels[1]);

    currentShader.setVec3("vNormal", glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

    currentShader.setVec3("camPos"     , Camera::instance().getPos());
    currentShader.setInt ("light_count", 1);
    currentShader.setPointLight("pl[0]", light1.getPointLight());

    myFloor.bindTexture(GL_TEXTURE1);
    currentShader.setInt("texture0", 1);
    myFloor.draw();
    
}

void Scene1::destroy() {

    // Cleans loaded-shaders from the Memory

    for (uint8_t i = 0; i < total_shaders; i++) {
        loaded_shaders[i].destroy();
    }
}