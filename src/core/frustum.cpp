#include <frustum.h>
#include <iostream>

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

bool Frustum::isMeshInside(const BoundingSphereComponent& sphere) {

    for (uint8_t plane = 0; plane < 6; plane++) {

        float d = glm::dot(glm::vec3(faces[plane].normal), sphere.center) + faces[plane].normal.w;
        if (d < -sphere.radius) return false;
    }
    
    return true;
}

void Frustum::update(const glm::mat4& viewProj) {

    // Extract 6 Faces of frustum from 'viewProj'
    const glm::vec4 col0 = viewProj[0];
    const glm::vec4 col1 = viewProj[1];
    const glm::vec4 col2 = viewProj[2];
    const glm::vec4 col3 = viewProj[3];

    faces[0].normal = col3 + col0;
    faces[1].normal = col3 - col0;
    faces[2].normal = col3 - col1;
    faces[3].normal = col3 + col1;
    faces[4].normal = col3 + col2;
    faces[5].normal = col3 - col2;

    for (auto& plane : faces) {
        plane.normal = glm::normalize(plane.normal);
    }
}