#pragma once

#include "ecs/entity.h"

BoundingAABBComponent createAABB(
    const TransformComponent& transform,
    const MeshComponent&      mesh
);

BoundingAABBComponent createAABB(
    const TransformComponent& transform,
    const ModelComponent&     model
);