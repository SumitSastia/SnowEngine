#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include <lights.h>

const uint8_t MAX_SHADERS = 8;

// ------------------------------ Function Delcarations ------------------------------ //



// ------------------------------ Classes -------------------------------------------- //

class Shader {

    unsigned int shaderProgram;
    std::string loadShaderFile(const char* path);

public:

    Shader(const char* vertPath, const char* fragPath);
    Shader(const char* vertPath, const char* geomPath, const char* fragPath);

    const unsigned int getShader() const { return shaderProgram; }

    void use() const;
    void destroy();

    // ------------------------------ Shader Uniform Setter ------------------------------ //

    void setBool (const char* target, const bool&  value);
    void setInt  (const char* target, const int&   value);
    void setFloat(const char* target, const float& value);
    void setVec3 (const char* target, const glm::vec3& vector);
    void setMat3 (const char* target, const glm::mat3& matrix);
    void setMat4 (const char* target, const glm::mat4& matrix);

    void setPointLight      (const std::string& target, const lights::PointLight& pl);
    void setSpotLight       (const std::string& target, const lights::SpotLight& sl);
    void setDirectionalLight(const std::string& target, const lights::DirectionalLight& dl);
};

// ------------------------------ Colors --------------------------------------------- //

namespace colors {

    const glm::vec3 RED    { 1.000f, 0.000f, 0.000f };
    const glm::vec3 ORANGE { 0.945f, 0.352f, 0.133f };
    const glm::vec3 YELLOW { 1.000f, 1.000f, 0.000f };
    const glm::vec3 GREEN  { 0.000f, 1.000f, 0.000f };
    const glm::vec3 BLUE   { 0.000f, 0.000f, 1.000f };
    const glm::vec3 PINK   { 0.890f, 0.239f, 0.580f };
    const glm::vec3 WHITE  { 1.000f, 1.000f, 1.000f };
    const glm::vec3 BLACK  { 0.000f, 0.000f, 0.000f };
    const glm::vec3 GRAY   { 0.392f, 0.392f, 0.392f };
}