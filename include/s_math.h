#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Matrix4 {

    glm::mat4 matrix_4x4;

public:
    
    Matrix4() : matrix_4x4(glm::mat4(1.0f)) {}
    Matrix4(float value) : matrix_4x4(glm::mat4(value)) {}

    void setMatrix(const glm::mat4& matrix) { matrix_4x4 = matrix; }
    
    void setPos(const glm::vec3& position);
    void translate(const glm::vec3& position);
    void scale(const glm::vec3& scale);
    void rotate(const float degrees, const glm::vec3& axis);

    const glm::mat4 getMatrix() const { return matrix_4x4; }
    const glm::mat3 getNormal() const { return glm::transpose(glm::inverse(glm::mat3(matrix_4x4))); }
    const glm::vec3 getPos()    const { return matrix_4x4[3]; }
};