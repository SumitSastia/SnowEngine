#pragma once

#include <ecs/entity.h>

class RenderSystem {

    static void bindCameraGlobals(const Shader* shader);
    static void bindPointLightGlobals(const EntityManager& entityManager, const ComponentManager& componentManager);
    static void bindFlashLightGlobals(const ComponentManager& componentManager);

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

    static void render      (const EntityManager& entityManager, const ComponentManager& componentManager);
    static void renderLights(const EntityManager& entityManager, const ComponentManager& componentManager); 
};