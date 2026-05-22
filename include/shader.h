#pragma once

#include <s_math.h>

#include <string>
#include <vector>

const uint8_t MAX_SHADERS = 8;

// ------------------------------ Function Declarations ------------------------------ //

namespace lights {

    struct DirectionalLight;
    struct PointLight;
    struct SpotLight;
}

// ------------------------------ Classes -------------------------------------------- //

class Shader {

    unsigned int shaderProgram;
    static std::string loadShaderFile(const char* path);
    static std::string preprocessFile(const char* path);

public:

    Shader(const std::string vertexStr, const std::string fragmentStr);

    Shader(const char* vertPath, const char* fragPath): 
        Shader(loadShaderFile(vertPath), loadShaderFile(fragPath)) {
    }

    Shader(const char* vertPath, const char* fragPath, const bool preprocess):
        Shader(loadShaderFile(vertPath), preprocessFile(fragPath)) {
    }

    Shader(const char* vertPath, const char* geomPath, const char* fragPath);

    const unsigned int getShader() const { return shaderProgram; }

    void use() const;
    void destroy();

    // ------------------------------ Shader Uniform Setter ------------------------------ //

    void setBool (const char* target, const bool   value) const;
    void setInt  (const char* target, const int&   value) const;
    void setFloat(const char* target, const float& value) const;
    void setVec3 (const char* target, const glm::vec3& vector) const;
    void setMat3 (const char* target, const glm::mat3& matrix) const;
    void setMat4 (const char* target, const glm::mat4& matrix) const;

    void setPointLight      (const std::string& target, const lights::PointLight& pl) const;
    void setSpotLight       (const std::string& target, const lights::SpotLight& sl) const;
    void setDirectionalLight(const std::string& target, const lights::DirectionalLight& dl) const;
};

class Texture2D {

    int width;
    int height;
    
    std::string type;
    unsigned int textureID;
    
public:

    Texture2D() :
        width(0),
        height(0),
        textureID(0) {
    }

    /*
    Loads the image and allocate it into the Memory.
    NOTE: "path" should start with '/' and rest should continue after the Main Directory (/SnowEngine).
    */
    void load(const char* path);

    void destroy();

    const unsigned int& getID()  const { return textureID; }
    const std::string& getType() const { return type; }
};

class CubeMap {
    
    unsigned int VBO, VAO, textureID;

public:

    CubeMap(const std::vector <std::string>& textureFaces);

    const unsigned int& get_VAO() const { return VAO; }
    const unsigned int& get_ID() const { return textureID; }
};

class Skybox {

    CubeMap* _cubemap;
    
    bool  isVisible;
    float lightIntensity;

public:

    Skybox(const std::vector <std::string>& textureFaces);

    void setVisibility(const bool visible) { isVisible = visible; }
    bool getVisibility() const { return isVisible; }

    void  setIntensity(const float intensity) { lightIntensity = intensity; }
    float getIntensity() const { return lightIntensity; }

    void bindTexture(const unsigned int textureUnit) const;

    void draw() const;
    void destroy();
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

    // const glm::vec3 RED    { 1.000f, 0.000f, 0.000f };
    // const glm::vec3 ORANGE { 0.882f, 0.100f, 0.118f };
    // const glm::vec3 YELLOW { 1.000f, 1.000f, 0.000f };
    // const glm::vec3 GREEN  { 0.000f, 1.000f, 0.000f };
    // const glm::vec3 BLUE   { 0.000f, 0.000f, 1.000f };
    // const glm::vec3 PINK   { 0.773f, 0.042f, 0.301f };
    // const glm::vec3 WHITE  { 1.000f, 1.000f, 1.000f };
    // const glm::vec3 BLACK  { 0.000f, 0.000f, 0.000f };
    // const glm::vec3 GRAY   { 0.133f, 0.133f, 0.133f };
}