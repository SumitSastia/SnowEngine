#include <frustum.h>

void Frustum::init(const glm::mat4& viewProj) {

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

bool Frustum::isMeshInside(const BoundingSphereComponent& sphere) {

    for (uint8_t plane = 0; plane < 6; plane++) {

        float d = glm::dot(faces[plane].normal, sphere.center) + faces[plane].distance;
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