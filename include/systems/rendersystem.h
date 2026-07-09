#pragma once

#include <iostream>
#include <ecs/entity.h>
#include <camera.h>
#include <utils/wireframe.h>

class Shader;

class RenderSystem {

    void bindCameraGlobals(const Shader& shader);
    void bindPointLightGlobals(const ECS& ecs);

    void draw(
        const MeshComponent&      mesh,
        const TransformComponent& transform,
        const MaterialComponent&  material
    );

    void draw(
        const MeshComponent&       mesh,
        const TransformComponent&  transform,
        const MaterialComponent&   material,
        const PointLightComponent& pointlight
    );

    void draw(
        const InstanceComponent& instance,
        const MaterialComponent& material
    );

    void drawGbuffer(
        const MeshComponent&      mesh,
        const TransformComponent& transform,
        const MaterialComponent&  material
    );

    void drawGbuffer(
        const MeshComponent&       mesh,
        const TransformComponent&  transform,
        const MaterialComponent&   material,
        const PointLightComponent& pointlight
    );

    void drawGbuffer(
        const InstanceComponent& instance,
        const MaterialComponent& material
    );

    void drawWireframe(
        const gfx::internal::Wireframe& wireframe,
        const BoundingSphereComponent& sphere
    );

    void drawWireframe(
        const gfx::internal::Wireframe& wireframe,
        const BoundingAABBComponent&    AABB
    );

    uint32_t lastTextureUnit;

public:

    RenderSystem(const Camera& camera):
        lastTextureUnit(0) {
    }

    static RenderSystem& instance() {
        static RenderSystem instance(Camera::instance());
        return instance;
    }

    void update(const float deltaTime);

    void render      (const ECS& ecs);
    void renderLights(const ECS& ecs);
    void renderTransparent(const ECS& ecs);

    void renderGbuffer(const ECS& ecs);
    void lightningPass(const ECS& ecs);
};

class ShadowSystem {

    glm::mat4 shadowProj;

    void drawShadow(
        const Shader&             shader,
        const MeshComponent&      mesh,
        const TransformComponent& transform
    );

    void drawShadowInstanced(
        const Shader&            shader,
        const InstanceComponent& instance
    );

public:

    ShadowSystem() {
        if (!init()) {
            std::cerr << "ERROR::FAILED TO INITIALIZE SHADOW-SYSTEM!" << std::endl;
        }
    }

    static ShadowSystem& instance() {
        static ShadowSystem instance {};
        return instance;
    }

    bool init();

    void render           (const ECS& ecs);
    void renderDirectional(const ECS& ecs);
};