#include <systems/update.h>

void updateTransform(ECS& ecs) {

    EntityManager&    entityManager    = ecs.entityManager;
    ComponentManager& componentManager = ecs.componentManager;

    const std::vector<Entity>& parents = ecs.view<TransformComponent, ChildComponent>();

    for (const Entity entity : parents) {

        TransformComponent& transform      = componentManager.get<TransformComponent>(entity);
        ChildComponent&     childComponent = componentManager.get<ChildComponent>(entity);

        transform.computePosition();

        for (const Entity child : childComponent.children) {

            TransformComponent& child_transform = componentManager.get<TransformComponent>(child);
            child_transform.position = transform.position + child_transform.local_position;

            child_transform.computeModel();
        }
    }
}