#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <s_math.h>

#include <string>
#include <vector>

const uint8_t MAX_SHADERS = 8;

// ------------------------------ Foward Declarations -------------------------------- //

namespace lights {

    struct DirectionalLight;
    struct PointLight;
    struct SpotLight;
}

namespace gfx::cubemap {

    class Cube {

        static unsigned int vbo, vao;
        Cube();

    public:

        static const unsigned int getVAO() {

            static Cube instance {};
            return instance.vao;
        }

        static void draw() {

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    };

};

// ------------------------------ Classes -------------------------------------------- //

class Shader {

    unsigned int shaderProgram;
    static std::string loadShaderFile(const char* path);
    static std::string preprocessFile(const char* path);

    /* 
    Compile Shaders
    @note Both parameters requires string of Shader Source.
    */
    Shader(const std::string vertexStr, const std::string fragmentStr);

public:

    Shader(const char* vertPath, const char* fragPath): 
        Shader(loadShaderFile(vertPath), loadShaderFile(fragPath)) {
    }

    // Preprocess header files, (if #include <> is present)
    Shader(const char* vertPath, const char* fragPath, const bool preprocess):
        Shader(loadShaderFile(vertPath), preprocessFile(fragPath)) {
    }

    /*
    Vertex -> Geometry -> Fragment
    @note No preprocess functionality.
    */
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

    void setPointLight      (const std::string& target, const lights::PointLight&       pl) const;
    void setPointLight      (const uint32_t&    target, const lights::PointLight&       pl) const;
    void setSpotLight       (const std::string& target, const lights::SpotLight&        sl) const;
    void setDirectionalLight(const std::string& target, const lights::DirectionalLight& dl) const;
};

class Texture2D {

    int width;
    int height;
    
    std::string type;
    unsigned int textureID;
    
public:

    Texture2D():
        width(0),
        height(0),
        textureID(0) {
    }

    Texture2D(uint textureID):
        width(0),
        height(0),
        textureID(textureID) {
    }

    /*
    Loads the image and allocate it into the Memory.
    NOTE: "path" should start with '/' and rest should continue after the Main Directory (/SnowEngine).
    @param format 0 - GL_RGB, 1 - GL_SRGB
    */
    void load(const char* path, const bool format = 0);
    void bind(const unsigned int textureUnit) const;

    void destroy();

    const unsigned int& getID()  const { return textureID; }
    const std::string& getType() const { return type; }
};

class CubeMap {
    
    unsigned int textureID;

public:

    /* 
    Cubemap with custom modifications 
    @param internal_format GL_RGB / GL_SRGB / GL_RGB16F (for HDR)
    @param type GL_UNSIGNED_BYTE / GL_FLOAT
    @param res_size Resolution of the Texture
    */
    CubeMap(const std::vector <std::string>& textureFaces, const uint16_t internal_format, const uint16_t type, const uint16_t res_size);

    /* Cubemap designed for Irradiance Maps. @param res_size Resolution of the Irradiance Map */
    CubeMap(const std::vector <std::string>& textureFaces, const uint16_t res_size) : CubeMap(textureFaces, GL_RGB, GL_UNSIGNED_BYTE, res_size) {}

    /* General purpose cubemap */
    CubeMap(const std::vector <std::string>& textureFaces) : CubeMap(textureFaces, GL_SRGB8, GL_UNSIGNED_BYTE, 0) {}

    void bindTexture(const unsigned int textureUnit) const;
    void destroy();
};

class Skybox {

    CubeMap* _cubeMap;
    CubeMap* _irradianceMap;
    
    bool  isVisible;
    float lightIntensity;

public:

    Skybox(const std::vector <std::string>& textureFaces);

    void setVisibility(const bool visible) { isVisible = visible; }
    bool getVisibility() const { return isVisible; }

    void  setIntensity(const float intensity) { lightIntensity = intensity; }
    float getIntensity() const { return lightIntensity; }

    void setIrradianceMap(const std::vector <std::string>& textureFaces);

    void bindTexture(const unsigned int textureUnit) const;
    void bindIrradiance(const unsigned int textureUni) const;

    void draw() const;
    void destroy();
};

// ------------------------------ Colors --------------------------------------------- //

namespace colors {

    // const glm::vec3 RED    { 1.000f, 0.000f, 0.000f };
    // const glm::vec3 ORANGE { 0.945f, 0.352f, 0.133f };
    // const glm::vec3 YELLOW { 1.000f, 1.000f, 0.000f };
    // const glm::vec3 GREEN  { 0.000f, 1.000f, 0.000f };
    // const glm::vec3 BLUE   { 0.000f, 0.000f, 1.000f };
    // const glm::vec3 PINK   { 0.890f, 0.239f, 0.580f };
    // const glm::vec3 WHITE  { 1.000f, 1.000f, 1.000f };
    // const glm::vec3 BLACK  { 0.000f, 0.000f, 0.000f };
    // const glm::vec3 GRAY   { 0.392f, 0.392f, 0.392f };

    const glm::vec3 RED    { 1.000f, 0.000f, 0.000f };
    const glm::vec3 ORANGE { 0.882f, 0.100f, 0.118f };
    const glm::vec3 YELLOW { 1.000f, 1.000f, 0.000f };
    const glm::vec3 GREEN  { 0.000f, 1.000f, 0.000f };
    const glm::vec3 BLUE   { 0.000f, 0.000f, 1.000f };
    const glm::vec3 PINK   { 0.773f, 0.042f, 0.301f };
    const glm::vec3 WHITE  { 1.000f, 1.000f, 1.000f };
    const glm::vec3 BLACK  { 0.000f, 0.000f, 0.000f };
    const glm::vec3 GRAY   { 0.133f, 0.133f, 0.133f };
}

// ------------------------------ Shaders -------------------------------------------- //

#define MAX_SHADERS 30

enum shaderNames {

    TEST3D,
    TEST2D,
    LIGHT3D,
    ALBEDO3D,
    ALBEDO2D,
    PHONG3D,
    PHONG2D,
    INSTANCE3D,
    INSTANCE2D,
    NORM_PHONG3D,
    NORM_PHONG2D,
    PARALLAX2D,
    PBR3D,
    PBR2D,
    NORMPBR3D,
    NORMPBR2D,
    INSTANCEPBR3D,
    INSTANCEPBR2D,

    SHADER_COUNT
};

struct shader_paths {

    std::string vert;
    std::string frag;
    bool preprocess;

    shader_paths(
        const std::string& vert,
        const std::string& frag,
        const bool preprocess = false
    ):
        vert(vert), frag(frag),
        preprocess(preprocess) {
    }
};

class Shaders {

    static std::vector <bool>    isLoaded;
    static std::vector <Shader*> loadedShaders;

    static std::vector <bool>         preprocess;
    static std::vector <shader_paths> path;

    static Shader* pointLightShadow;
    static Shader* directLightShadow;

    static Shader* pointLightShadow_instanced;
    static Shader* directLightShadow_instanced;

public:

    static bool initShaders();

    static Shader* getPointLightShadow() { return pointLightShadow; }
    static Shader* getDirectLightShadow() { return directLightShadow; }

    static Shader* getPointLightShadow_Instanced() { return pointLightShadow_instanced; }
    static Shader* getDirectLightShadow_Instanced() { return directLightShadow_instanced; }

    static Shader* get(shaderNames shader);
    static uint32_t totat() { return SHADER_COUNT; }
};