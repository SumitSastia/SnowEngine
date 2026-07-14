#include "crosshair.h"

#include <glm/gtx/quaternion.hpp>

#include "core/renderer.h"
#include "core/img/texture.h"
#include "core/shader.h"

glm::mat4 Crosshair::model1 { 1.0f };
glm::mat4 Crosshair::model2 { 1.0f };

void Crosshair::init() {

    float line_length = 0.02f;
    float aspectRatio = (float)WIN_W / (float)WIN_H;

    // Vertical Line
    const glm::vec3 point3 = glm::vec3(0.0f,-line_length, 0.0f);
    const glm::vec3 point4 = glm::vec3(0.0f, line_length, 0.0f);

    glm::vec3 direction = point4 - point3;
    float     length    = glm::length(direction);
    
    direction = glm::normalize(direction);
    glm::quat rotation  = glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), direction);
    
    model2  = glm::translate(model2, point3);
    model2 *= glm::mat4_cast(rotation);
    model2  = glm::scale(model2, glm::vec3(length, 1.0f, 1.0f));

    // Horizontal Line
    const glm::vec3 point1 = glm::vec3(-line_length / aspectRatio, 0.0f, 0.0f);
    const glm::vec3 point2 = glm::vec3( line_length / aspectRatio, 0.0f, 0.0f);

    direction = point2 - point1;
    length    = glm::length(direction);
    direction = glm::normalize(direction);
    rotation  = glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), direction);

    model1  = glm::translate(model1, point1);
    model1 *= glm::mat4_cast(rotation);
    model1  = glm::scale(model1, glm::vec3(length, 1.0f, 1.0f));
}

void Crosshair::render(const glm::vec3& color) {

    // const Shader& shader = *ShaderManager::getLineShader();
    const Shader& shader = ShaderManager::getUtil(gfx::shader::LINE);

    shader.use();
    shader.setVec3("color", color);

    shader.setMat4("finalMatrix", model1);
    glBindVertexArray(Line::VAO);
    glDrawArrays(GL_LINES, 0, 2);

    shader.setMat4("finalMatrix", model2);
    glBindVertexArray(Line::VAO);
    glDrawArrays(GL_LINES, 0, 2);
}