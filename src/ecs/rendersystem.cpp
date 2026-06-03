#include <ecs/rendersystem.h>

#include <camera.h>
#include <shader.h>
#include <shapes.h>
#include <renderer.h>
#include <debug.h>
#include <lights.h>

#include <iostream>

// void RenderSystem::update(ECS& ecs) {

//     ecs.total_visible  = ecs.visibleEntities.size();
//     ecs.total_emissive = ecs.emissiveEntities.size();
// }

void RenderSystem::bindCameraGlobals(const Shader* shader) {

    shader->setMat4("projection", Camera::instance().getPerspective());
    shader->setMat4("view",       Camera::instance().getView());
    shader->setVec3("camPos",     Camera::instance().getPos());
}

void RenderSystem::bindPointLightGlobals(const EntityManager& entityManager, const ComponentManager& componentManager) {

    const uint32_t light_count = entityManager.emissiveEntities.size();

    for (const Shader* shader : componentManager.uniqueShaders) {

        shader->use();
        shader->setInt("light_count", light_count);
        shader->setFloat("far_plane", 25.0f);

        for (uint32_t entity = 0; entity < light_count; entity++) {

            const Entity& light = entityManager.emissiveEntities[entity];
            
            const PointLightComponent& pointlight = componentManager.arr_light[light];
            const TransformComponent&  transform  = componentManager.arr_transform[light];

            const std::string e = "pl[" + std::to_string(entity) + "]";

            shader->setVec3((e + ".position").c_str(), transform.position);
            shader->setVec3((e + ".color").c_str(),    pointlight.color);

            shader->setFloat((e + ".constant").c_str(),  pointlight.constant);
            shader->setFloat((e + ".linear").c_str(),    pointlight.linear);
            shader->setFloat((e + ".quadratic").c_str(), pointlight.quadratic);
        }
    }
}

void RenderSystem::bindFlashLightGlobals(const ComponentManager& componentManager) {

    for (const Shader* shader : componentManager.uniqueShaders) {

        shader->use();
        shader->setBool("useSpotLight", DefaultLights::instance().flashlight.isVisible);
        shader->setSpotLight("sl", DefaultLights::instance().flashlight);

        bindCameraGlobals(shader);
    }
}

void RenderSystem::render(const EntityManager& entityManager, const ComponentManager& componentManager) {

    bindPointLightGlobals(entityManager, componentManager);
    bindFlashLightGlobals(componentManager);

    for (const Entity& entity : entityManager.visibleEntities) {

        if (
            componentManager.has_mesh[entity] &&
            componentManager.has_transform[entity] &&
            componentManager.has_material[entity]
        ) {

            const MeshComponent&      mesh      = componentManager.arr_mesh[entity];
            const TransformComponent& transform = componentManager.arr_transform[entity];
            const MaterialComponent&  material  = componentManager.arr_material[entity];
    
            draw(mesh, transform, material);
        }

        if (
            componentManager.has_instance[entity] && 
            componentManager.has_material[entity]
        ) {

            const InstanceComponent& instance = componentManager.arr_instance[entity];
            const MaterialComponent& material = componentManager.arr_material[entity];

            draw(instance, material);
        }
    }
}

void RenderSystem::renderLights(const EntityManager& entityManager, const ComponentManager& componentManager) {

    for (const Entity& entity : entityManager.emissiveEntities) {

        const MeshComponent&       mesh       = componentManager.arr_mesh[entity];
        const TransformComponent&  transform  = componentManager.arr_transform[entity];
        const MaterialComponent&   material   = componentManager.arr_material[entity];
        const PointLightComponent& pointlight = componentManager.arr_light[entity];

        draw(mesh, transform, material, pointlight);
    }
}

void RenderSystem::draw(
    const MeshComponent&      mesh,
    const TransformComponent& transform,
    const MaterialComponent&  material
) {
    const Shader* shader = material.shader;

    shader->use();
    shader->setMat4("model",        transform.model);
    shader->setMat3("normalMatrix", transform.model.getNormal());

    // bindCameraGlobals(shader);

    if (material.albedo) {
        shader->setInt("albedo", 0);
        material.albedo->bind(0);
    }

    mesh.shape.draw();
}

void RenderSystem::draw(
    const MeshComponent&       mesh,
    const TransformComponent&  transform,
    const MaterialComponent&   material,
    const PointLightComponent& pointlight
) {
    const Shader* shader = material.shader;

    shader->use();
    shader->setMat4("model", transform.model);
    shader->setVec3("lightColor", pointlight.color);

    mesh.shape.draw();
}

void RenderSystem::draw(
    const InstanceComponent& instance,
    const MaterialComponent& material
) {

    material.shader->use();

    if (material.albedo) {
        material.shader->setInt("albedo", 0);
        material.albedo->bind(0);
    }

    instance.draw();
}