#pragma once

#include <shapes.h>
#include <lights.h>
#include <shader.h>
#include <frame.h>
#include <s_math.h>
#include <model.h>
#include <ibl.h>
#include <s_time.h>
#include <ecs/rendersystem.h>

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
    virtual void renderGbuffer() const = 0;
    virtual void renderDeferred(const Shader& currentShader) const = 0;
    virtual void renderLight() const = 0;
    virtual void renderDirectShadow() const = 0;
    virtual void renderPointShadow() const = 0;
    virtual void renderSkybox() const = 0;
    virtual void renderDebug() const = 0;
    virtual void destroy() = 0;
};

class Scene1 : public Scene {

    // Entities
    Shape myFloor;
    Shape myGround;
    Shape myCube;
    Shape advCube;
    Shape myWall;

    Model3D* mySphere;
    ShapeInstanced cubes;

    std::vector <Matrix4> entityModels;

    // Light
    uint8_t light_count;
    std::vector <LightSource*> lights;
    std::vector <Matrix4*> lightModels;

    // Shadow
    glm::mat4 shadowProj;
    std::vector <PointShadowFrame*> shadowFrames;

    // Light Space - Directional Light
    glm::mat4 lightSpace;
    DirectShadowFrame* directFrame;

    // Skybox
    Skybox* _skybox;

    // ECS
    // ECS ecs;
    Entity cube;

    // Debug
    DebugFrame* debugFrame;

    // FrameBuffers
    // IBLFrame* ibl_frame;

public:

    Scene1() {
        this->init();
    }

    void init() override;
    void input(GLFWwindow* window, const float& delta_time) override;
    void update(const float& delta_time) override;
    void render() const override;
    void renderGbuffer() const override;
    void renderDeferred(const Shader& currentShader) const override;
    void renderLight() const override;
    void renderDirectShadow() const override;
    void renderPointShadow() const override;
    void renderSkybox() const override;
    void renderDebug() const override;
    void destroy() override;
};

class Scene2 : public Scene {

    Entity wood_box;
    Entity box2;
    Entity floor;
    Entity wall;
    Entity light1;
    Entity light2;
    Entity cubes;

    EntityManager    entityManager;
    ComponentManager componentManager;

public:

    Scene2() { init(); }

    void init() override;
    void input(GLFWwindow* window, const float& delta_time) override;
    void update(const float& delta_time) override {}
    void render() const override;
    void renderGbuffer() const override {}
    void renderDeferred(const Shader& currentShader) const override {}
    void renderLight() const override;
    void renderDirectShadow() const override {}
    void renderPointShadow() const override {}
    void renderSkybox() const override {}
    void renderDebug() const override {}
    void destroy() override {}
};