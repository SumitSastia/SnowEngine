#include <lights.h>
#include <shapes.h>
#include <camera.h>

DefaultLights::DefaultLights() {

    cubelight.position = glm::vec3(3.0f, 1.5f,-3.0f);
    cubelight.color    = glm::vec3(1.0f, 1.0f, 1.0f);

    cubelight.constant  = 1.0f;
    cubelight.linear    = 0.001f;
    cubelight.quadratic = 0.016f;

    sunlight.color     = glm::vec3(0.99f, 0.82f, 0.66f);
    sunlight.direction = glm::vec3(1.0f, 1.0f, 1.0f);

    flashlight.position  = glm::vec3(0.0f, 0.0f, 3.0f);
    flashlight.direction = glm::vec3(0.0f, 0.0f, 0.0f);
    flashlight.color     = glm::vec3(1.0f, 1.0f, 1.0f);

    flashlight.isVisible = false;

    flashlight.cutOffangle = glm::cos(glm::radians(12.5f));
    flashlight.outerCutOff = glm::cos(glm::radians(17.5f));

    flashlight.constant  = 1.0f;
    flashlight.linear    = 0.001f;
    flashlight.quadratic = 0.016f;
}

void DefaultLights::update() {

    flashlight.position  = Camera::instance().getPos();
    flashlight.direction = Camera::instance().getTarget();
}

// --------------------------------------------------------------------------------------- //

LightSource::LightSource() {

    src      = DefaultLights::instance().cubelight;
    srcShape = new Shape(DefaultShapes::instance().cube.copy());
}

void LightSource::setX(const float& x) {
    src.position.x = src.position.x + x; 
}

void LightSource::setY(const float& y) {
    src.position.y = src.position.y + y; 
}

void LightSource::setZ(const float& z) {
    src.position.z = src.position.z + z; 
}

void LightSource::draw() const {
    srcShape->draw();
}