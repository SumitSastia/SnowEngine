#pragma once

#include <ecs/entity.h>

class RenderSystem {

    static void bindCameraGlobals(const Shader* shader);
    static void bindPointLightGlobals(const EntityManager& entityManager, const ComponentManager& componentManager);

    // For Light
    // static void draw(const EntityMesh& mesh, const Transform& transform, const MaterialLight& material);

    // For Phong
    // static void draw(const EntityMesh& mesh, const Transform& transform, const MaterialPhong& material);

    static void draw(
        const MeshComponent& mesh,
        const TransformComponent& transform,
        const MaterialComponent& material
    );

    static void draw(
        const MeshComponent& mesh,
        const TransformComponent& transform,
        const MaterialComponent& material,
        const PointLightComponent& pointlight
    );

public:

    static RenderSystem& instance() {

        static RenderSystem instance {};
        return instance;
    }

    static void render(const EntityManager& entityManager, const ComponentManager& componentManager);
    static void renderLights(const EntityManager& entityManager, const ComponentManager& componentManager);
};