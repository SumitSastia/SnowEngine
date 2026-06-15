#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <ecs/component.h>

namespace gfx::internal {

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct Plane {
        glm::vec3 normal;
        float distance;
    };
};

class Frustum {

    // 0 - Left
    // 1 - Right
    // 2 - Top
    // 3 - Bottom
    // 4 - Near
    // 5 - Far

    std::vector <gfx::internal::Plane> faces;

    void init(const glm::mat4& viewProj);

public:

    Frustum(const glm::mat4& viewProj):
        faces(6)
    {
        init(viewProj);
    }

    bool isMeshInside(const BoundingSphereComponent& sphere);

    // Extracts 6 Faces(Planes) of the Frustum
    void update(const glm::mat4& viewProj);
};