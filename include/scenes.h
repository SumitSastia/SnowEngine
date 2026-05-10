#pragma once

#include <shapes.h>
#include <lights.h>
#include <shader.h>

class Scene {

protected:

    uint8_t  total_shaders;
    uint16_t total_entities;

    std::vector <Shader> loaded_shaders;
    std::vector <Entity> loaded_entities;

    Scene() : total_shaders(0), total_entities(0) {}

public:

    virtual void init() = 0;
    virtual void input(GLFWwindow* window, const float& delta_time) = 0;
    virtual void render() const = 0;
    virtual void destroy() = 0;
};

class Scene1 : public Scene {

    // Entities

    Shape myFloor;
    Shape myCube;
    ShapeInstanced cubes;
    LightSource light1;

    std::vector <glm::mat4> entityModels;

public:

    Scene1() {
        this->init();
    }

    void init() override;
    void input(GLFWwindow* window, const float& delta_time) override;
    void render() const override;
    void destroy() override;
};