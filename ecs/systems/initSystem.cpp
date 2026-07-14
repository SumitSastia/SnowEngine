#include "init.h"

BoundingAABBComponent createAABB(
    const TransformComponent& transform,
    const MeshComponent&      mesh
) {
    BoundingAABBComponent AABB { transform.position, mesh.minCorner, mesh.maxCorner };
    AABB.recompute(transform.model);

    return AABB;
}

BoundingAABBComponent createAABB(
    const TransformComponent& transform,
    const ModelComponent&     model
) {
    BoundingAABBComponent AABB { transform.position, model.minCorner, model.maxCorner };
    AABB.recompute(transform.model);

    return AABB;
}