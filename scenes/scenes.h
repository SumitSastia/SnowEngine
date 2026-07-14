#pragma once

#include "core/lights.h"
#include "core/frame/frame.h"
#include "core/shader.h"
#include "core/model/model.h"
#include "core/img/ibl.h"
#include "core/gfx/particles.h"

#include "math/matrix.h"
#include "include/s_time.h"

#include "ecs/systems/init.h"
#include "ecs/systems/rendersystem.h"

namespace scene_var {
    const float speed = 3.0f;
};

class Scene {

protected:

    uint8_t  total_shaders;
    uint16_t total_entities;

    std::vector <Shader> loaded_shaders;
    std::vector <Entity> loaded_entities;

    Scene() : total_shaders(0), total_entities(0) {}

public:

    virtual void init()                                             = 0;
    virtual void input(GLFWwindow* window, const float& delta_time) = 0;
    virtual void update(const float delta_time)                     = 0;
    virtual void render() const                                     = 0;
    virtual void renderGbuffer() const                              = 0;
    virtual void renderDeferred() const                             = 0;
    virtual void renderLight() const                                = 0;
    virtual void renderDirectShadow() const                         = 0;
    virtual void renderPointShadow() const                          = 0;
    virtual void renderSkybox() const                               = 0;
    virtual void renderDebug() const                                = 0;
    virtual void destroy()                                          = 0;

    virtual void renderParticles(const TextureHandle depthTexture = 0) const {}
};

// class Scene1 : public Scene {

//     // Entities
//     Shape myFloor;
//     Shape myGround;
//     Shape myCube;
//     Shape advCube;
//     Shape myWall;

//     Model3D* mySphere;
//     ShapeInstanced cubes;

//     std::vector <Matrix4> entityModels;

//     // Light
//     uint8_t light_count;
//     std::vector <LightSource*> lights;
//     std::vector <Matrix4*> lightModels;

//     // Shadow
//     glm::mat4 shadowProj;
//     std::vector <PointShadowFrame*> shadowFrames;

//     // Light Space - Directional Light
//     glm::mat4 lightSpace;
//     DirectShadowFrame* directFrame;

//     // Skybox
//     // Skybox* _skybox;

//     // ECS
//     // ECS ecs;
//     Entity cube;

//     // Debug
//     DebugFrame* debugFrame;

//     // FrameBuffers
//     // IBLFrame* ibl_frame;

// public:

//     Scene1() {
//         this->init();
//     }

//     void init() override;
//     void input(GLFWwindow* window, const float& delta_time) override;
//     void update(const float delta_time) override;
//     void render() const override;
//     void renderGbuffer() const override;
//     void renderDeferred(const Shader& currentShader) const override;
//     void renderLight() const override;
//     void renderDirectShadow() const override;
//     void renderPointShadow() const override;
//     void renderSkybox() const override;
//     void renderDebug() const override;
//     void destroy() override;
// };

class Scene2 : public Scene {

    Entity wood_box;
    Entity box2;
    Entity floor;
    Entity wall;
    Entity light1;
    Entity light2;
    Entity cubes;
    Entity sphere;
    Entity brickWall;
    Entity sun;
    Entity headcam;
    Entity gun;
    Entity mainCamera;
    Entity campfire;
    Entity flames;
    Entity crowd[1000];

    ECS ecs;
    Environment* env;

    ParticleEmitter fire_emitter;
    ParticleEmitter rain_emitter;
    ParticleEmitter bullet_emitter;
    ParticleEmitter smoke_emitter;

    glm::vec3 gun_muzzle;

public:

    Scene2(): env(nullptr) {
        init();
    }

    void init() override;
    void input(GLFWwindow* window, const float& delta_time) override;
    void update(const float delta_time) override;
    void render() const override;
    void renderGbuffer() const override;
    void renderDeferred() const override;
    void renderLight() const override;
    void renderDirectShadow() const override;
    void renderPointShadow() const override;
    void renderSkybox() const override {}
    void renderDebug() const override {}
    void destroy() override {}

    void renderParticles(const TextureHandle depthTexture = 0) const;
};