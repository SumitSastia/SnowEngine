#include <iostream>
#include "frustum.h"

void Frustum::init(const glm::mat4& viewProj) {

    // Extract 6 Faces of frustum from 'viewProj'
    const glm::vec4 col0 = viewProj[0];
    const glm::vec4 col1 = viewProj[1];
    const glm::vec4 col2 = viewProj[2];
    const glm::vec4 col3 = viewProj[3];

    glm::vec4 row0(
        viewProj[0][0],
        viewProj[1][0],
        viewProj[2][0],
        viewProj[3][0]
    );

    glm::vec4 row1(
        viewProj[0][1],
        viewProj[1][1],
        viewProj[2][1],
        viewProj[3][1]
    );

    glm::vec4 row2(
        viewProj[0][2],
        viewProj[1][2],
        viewProj[2][2],
        viewProj[3][2]
    );

    glm::vec4 row3(
        viewProj[0][3],
        viewProj[1][3],
        viewProj[2][3],
        viewProj[3][3]
    );

    faces[0].normal = row3 + row0; // left
    faces[1].normal = row3 - row0; // right
    faces[2].normal = row3 - row1; // top
    faces[3].normal = row3 + row1; // bottom
    faces[4].normal = row3 + row2; // near
    faces[5].normal = row3 - row2; // far

    for (auto& plane : faces) {
        plane.normal /= glm::length(glm::vec3(plane.normal));
    }
}

bool Frustum::isMeshInside(const BoundingSphereComponent& sphere) const {

    for (const auto& plane : faces) {

        float d = glm::dot(glm::vec3(plane.normal), sphere.center) + plane.normal.w;
        if (d < -sphere.radius) return false;
    }
    
    return true;
}

bool Frustum::isMeshInside(const BoundingAABBComponent& AABB) const {

    glm::vec3 positive;
    
    for (const auto& plane : faces) {

        positive.x = (plane.normal.x >= 0.0f)? AABB.max.x : AABB.min.x;
        positive.y = (plane.normal.y >= 0.0f)? AABB.max.y : AABB.min.y;
        positive.z = (plane.normal.z >= 0.0f)? AABB.max.z : AABB.min.z;
    
        float d = glm::dot(glm::vec3(plane.normal), positive) + plane.normal.w;
        if (d < 0.0f) return false;
    }
    
    return true;
}