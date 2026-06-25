#include <systems/update.h>

void updateTransform(ECS& ecs) {

    EntityManager&    entityManager    = ecs.entityManager;
    ComponentManager& componentManager = ecs.componentManager;

    const std::vector<Entity>& cameras = ecs.view<TransformComponent, CameraComponent>();
    const std::vector<Entity>& parents = ecs.view<TransformComponent, ChildComponent>();

    for (const Entity entity : cameras) {

        CameraComponent& cameraComp   = componentManager.get<CameraComponent>(entity);
        TransformComponent& transform = componentManager.get<TransformComponent>(entity);

        transform.position = cameraComp.camera.getPos();
        transform.computeModel();
    }

    for (const Entity entity : parents) {

        TransformComponent& transform      = componentManager.get<TransformComponent>(entity);
        ChildComponent&     childComponent = componentManager.get<ChildComponent>(entity);

        transform.computePosition();

        for (const Entity child : childComponent.children) {

            TransformComponent& child_transform = componentManager.get<TransformComponent>(child);
            child_transform.position = transform.position + child_transform.local_position;

            child_transform.computeModel();
            child_transform.isVisible = transform.isVisible;
        }
    }

    const std::vector<Entity>& entities = ecs.view<TransformComponent, BoundingAABBComponent>();

    for (const Entity& entity : entities) {

        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
        BoundingAABBComponent&    AABB      = componentManager.get<BoundingAABBComponent>(entity);

        AABB.recompute(transform.model);
    }
}