#include <scenes.h>
#include <camera.h>

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
            "../shaders/planes/plane.frag"
        ),
        Shader(
            "../shaders/instancedCubes/texture.vert",
            "../shaders/instancedCubes/texture.frag"
        )
    };

    loaded_entities = {

        DefaultShapes::instance().cube,
        DefaultShapes::instance().square,
        DefaultShapes::instance().cubeInstanced
    };

    myCube = DefaultShapes::instance().cube;
    square = DefaultShapes::instance().square;
    cubes  = DefaultShapes::instance().cubeInstanced;
}

void Scene1::render() const {

    const unsigned int shader = loaded_shaders[0].getShader();

    glUseProgram(shader);

    Renderer::setMat4(shader, "projection", Camera::instance().getPerspective());
    Renderer::setMat4(shader, "view"      , Camera::instance().getView());

    glm::mat4 objectModel(1.0f);
    Renderer::setMat4(shader, "model", objectModel);

    myCube.bindTexture(GL_TEXTURE0);
    myCube.draw();
}

void Scene1::destroy() {

    // Cleans loaded-shaders from the Memory

    for (uint8_t i = 0; i < total_shaders; i++) {
        loaded_shaders[i].destroy();
    }
}