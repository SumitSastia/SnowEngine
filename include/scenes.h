#pragma once

#include <shapes.h>
#include <lights.h>
#include <shader.h>
#include <frame.h>

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
    virtual void update(const float& delta_time) = 0;
    virtual void render() const = 0;
    virtual void renderDirectShadow() const = 0;
    virtual void renderPointShadow() const = 0;
    virtual void destroy() = 0;
};

class Scene1 : public Scene {

    // Entities
    Shape myFloor;
    Shape myCube;
    ShapeInstanced cubes;

    std::vector <Entity>    entities;
    std::vector <glm::mat4> entityModels;

    // Light
    uint8_t light_count;
    std::vector <LightSource*> lights;

    // Shadow
    glm::mat4 shadowProj;
    std::vector <PointShadowFrame*> shadowFrames;

    // Light Space - Directional Light
    glm::mat4 lightSpace;
    glm::mat4 lightProjection;

    DirectShadowFrame* directFrame;

public:

    Scene1() {
        this->init();
    }

    void init() override;
    void input(GLFWwindow* window, const float& delta_time) override;
    void update(const float& delta_time) override;
    void render() const override;
    void renderDirectShadow() const override;
    void renderPointShadow() const override;
    void destroy() override;
};