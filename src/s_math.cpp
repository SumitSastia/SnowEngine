#include <s_math.h>

void Matrix4::translate(const glm::vec3& position) {
    matrix_4x4 = glm::translate(matrix_4x4, position);
}

void Matrix4::rotate(const float degrees, const glm::vec3& axis) {
    matrix_4x4 = glm::rotate(matrix_4x4, glm::radians(degrees), axis);
}

void Matrix4::scale(const glm::vec3& scale) {
    matrix_4x4 = glm::scale(matrix_4x4, scale);
}

void Matrix4::setPos(const glm::vec3& position) {
    matrix_4x4 = glm::translate(glm::mat4(1.0f), position);
}