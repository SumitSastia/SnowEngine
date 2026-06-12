#pragma once

#include <ecs/entity.h>

class RenderSystem {

    static void bindCameraGlobals(const Shader* shader);
    static void bindPointLightGlobals(const ECS& ecs);
    // static void bindFlashLightGlobals(const ComponentManager& componentManager);

    static void draw(
        const MeshComponent&      mesh,
        const TransformComponent& transform,
        const MaterialComponent&  material
    );

    static void draw(
        const MeshComponent&       mesh,
        const TransformComponent&  transform,
        const MaterialComponent&   material,
        const PointLightComponent& pointlight
    );

    static void draw(
        const InstanceComponent& instance,
        const MaterialComponent& material
    );

    static void drawGbuffer(
        const MeshComponent&      mesh,
        const TransformComponent& transform,
        const MaterialComponent&  material
    );

    static void drawGbuffer(
        const MeshComponent&       mesh,
        const TransformComponent&  transform,
        const MaterialComponent&   material,
        const PointLightComponent& pointlight
    );

    static void drawGbuffer(
        const InstanceComponent& instance,
        const MaterialComponent& material
    );

    static uint8_t lastTextureUnit;

public:

    static RenderSystem& instance() {

        static RenderSystem instance {};
        return instance;
    }

    static void update(const float deltaTime);

    static void render      (const ECS& ecs);
    static void renderLights(const ECS& ecs);

    static void renderGbuffer(const ECS& ecs);
    static void lightningPass(const ECS& ecs, const Shader* shader);
};

class ShadowSystem {

    static glm::mat4 shadowProj;

    static void drawShadow(
        const Shader&             shader,
        const MeshComponent&      mesh,
        const TransformComponent& transform
    );

    static void drawShadowInstanced(
        const Shader&            shader,
        const InstanceComponent& instance
    );

public:

    static bool init();

    static void render           (const ECS& ecs);
    static void renderDirectional(const ECS& ecs);
};