#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Matrix4 {

    glm::mat4 matrix_4x4;

public:
    
    Matrix4() : matrix_4x4(glm::mat4(1.0f)) {}

    void setMatrix(const glm::mat4 matrix) { matrix_4x4 = matrix; }

    void translate(const glm::vec3& position);
    void rotate(const float& degrees, const glm::vec3& axis);
    void scale(const glm::vec3& scale);

    const glm::mat4 getMatrix() const { return matrix_4x4; }
    const glm::vec3 getPos() const { return matrix_4x4[3]; }
};