#pragma once

#include <ecs/entity.h>

class RenderSystem {

    static void bindCameraGlobals(const Shader* shader);
    static void bindPointLightGlobals(const EntityManager& entityManager, const ComponentManager& componentManager);
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

public:

    static RenderSystem& instance() {

        static RenderSystem instance {};
        return instance;
    }

    static void update(const float deltaTime);

    static void render      (const EntityManager& entityManager, const ComponentManager& componentManager);
    static void renderLights(const EntityManager& entityManager, const ComponentManager& componentManager);
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

    static void render           (const EntityManager& entityManager, const ComponentManager& componentManager);
    static void renderDirectional(const EntityManager& entityManager, const ComponentManager& componentManager);
};