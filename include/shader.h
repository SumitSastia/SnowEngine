#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

const uint8_t MAX_SHADERS = 8;

// ------------------------------ Foward Declarations -------------------------------- //

namespace lights {

    struct DirectionalLight;
    struct PointLight;
    struct SpotLight;
}

// ------------------------------ Classes -------------------------------------------- //

class Shader {

    unsigned int shaderProgram = 0u;
    
    static std::string loadShaderFile(const char* path);
    static std::string preprocessFile(const char* path);

    /* 
    Compile Shaders
    @note Both parameters requires string of Shader Source.
    */
    // Shader(const std::string vertexStr, const std::string fragmentStr);

public:

    Shader() = default;
    
    void loadFromFile(const char* vertPath, const char* fragPath, const bool preprocess = false);
    void loadFromFile(const char* vertPath, const char* geomPath, const char* fragPath);

    void loadFromString(const std::string& vertStr, const std::string& fragStr);
    void loadFromString(const std::string& vertStr, const std::string& geomStr, const std::string& fragStr);

    // Shader(const char* vertPath, const char* fragPath): 
    //     Shader(loadShaderFile(vertPath), loadShaderFile(fragPath)) {
    // }

    // // Preprocess header files, (if #include <> is present)
    // Shader(const char* vertPath, const char* fragPath, const bool preprocess):
    //     Shader(loadShaderFile(vertPath), preprocessFile(fragPath)) {
    // }

    /*
    Vertex -> Geometry -> Fragment
    @note No preprocess functionality.
    */
    // Shader(const char* vertPath, const char* geomPath, const char* fragPath);

    const unsigned int& getShader() const { return shaderProgram; }

    void use() const;
    void destroy();

    // ------------------------------ Shader Uniform Setter ------------------------------ //

    void setBool (const char* target, const bool   value) const;
    void setInt  (const char* target, const int&   value) const;
    void setFloat(const char* target, const float& value) const;
    void setVec3 (const char* target, const glm::vec3& vector) const;
    void setVec4 (const char* target, const glm::vec4& vector) const;
    void setMat3 (const char* target, const glm::mat3& matrix) const;
    void setMat4 (const char* target, const glm::mat4& matrix) const;

    void setPointLight      (const std::string& target, const lights::PointLight&       pl) const;
    void setPointLight      (const uint32_t&    target, const lights::PointLight&       pl) const;
    void setSpotLight       (const std::string& target, const lights::SpotLight&        sl) const;
    void setDirectionalLight(const std::string& target, const lights::DirectionalLight& dl) const;
};

// ------------------------------ Colors --------------------------------------------- //

namespace colors {

    const glm::vec3 RED        { 1.000f, 0.000f, 0.000f };
    const glm::vec3 PINK       { 0.773f, 0.042f, 0.301f };
    const glm::vec3 ORANGE     { 0.882f, 0.100f, 0.118f };
    const glm::vec3 YELLOW     { 1.000f, 1.000f, 0.000f };
    const glm::vec3 GOLD       { 0.937f, 0.749f, 0.015f };
    const glm::vec3 GREEN      { 0.000f, 1.000f, 0.000f };
    const glm::vec3 BLUE       { 0.000f, 0.000f, 1.000f };
    const glm::vec3 LIGHT_BLUE { 0.564f, 0.835f, 1.000f };
    const glm::vec3 VIOLET     { 0.500f, 0.000f, 1.000f };
    const glm::vec3 WHITE      { 1.000f, 1.000f, 1.000f };
    const glm::vec3 BLACK      { 0.000f, 0.000f, 0.000f };
    const glm::vec3 BROWN      { 0.439f, 0.258f, 0.078f };
    const glm::vec3 GRAY       { 0.133f, 0.133f, 0.133f };
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
    PBR3D_COLORED,
    PBR2D_COLORED,
    NORMPBR3D,
    NORMPBR2D,
    INSTANCEPBR3D,
    INSTANCEPBR2D,
    ENVIRONMENT,
    GBUFFER3D,
    GBUFFER2D,
    INSTANCE_GBUFFER3D,
    GBUFFERNORM_3D,
    GBUFFERNORM_2D,
    WIREFRAME,
    NORMPBR2D_BACKFACE,

    SHADER_COUNT
};

namespace gfx::shader {

    enum shaders : uint8_t {

        ALBEDO,
        PHONG,
        PHONG_NORM,
        PHONG_INSTANCE,
        PBR,
        PBR_NORM,
        PBR_INSTANCE,
        GBUFFER,
        GBUFFER_NORM,
        GBUFFER_INSTANCE,
        PARALLAX,

        SHADER_TOTAL_COUNT
    };

    enum shadersUtil : uint8_t {
        
        LINE,
        LIGHT,
        WIREFRAME,
        ENVIRONMENT,
        PARTICLE_COLORED,
        PARTICLE_TEXTURED,

        SHADERUTIL_TOTAL_COUNT
    };

    enum special {
        COLORED_PARTICLE,
        TETXURED_PARTICLE,

        TOTAL_SPECIAL_SHADERS
    };
};

struct ShaderPath {

    std::string vert;
    std::string frag;
    bool preprocess;

    ShaderPath(
        const std::string& vert,
        const std::string& frag,
        const bool preprocess = false
    ):
        vert(vert), frag(frag),
        preprocess(preprocess) {
    }
};

class ShaderManager {

    static std::vector <bool>    isLoaded;
    static std::vector <Shader*> loadedShaders;

    static std::vector <bool>       preprocess;
    static std::vector <ShaderPath> path;

    static std::vector <Shader*> specialShaders;

    static Shader* pointLightShadow;
    static Shader* directLightShadow;

    static Shader* pointLightShadow_instanced;
    static Shader* directLightShadow_instanced;

    // static Shader* lineShader;
    // static Shader* particleShader;

    static std::vector <ShaderPath> path3D;
    static std::vector <ShaderPath> path2D;
    static std::vector <ShaderPath> pathUtil;

    static std::vector <Shader> lShaders3D;
    static std::vector <Shader> lShaders2D;
    static std::vector <Shader> lShadersUtil;

public:

    static bool initShaders();

    static Shader* getPointLightShadow() { return pointLightShadow; }
    static Shader* getDirectLightShadow() { return directLightShadow; }

    static Shader* getPointLightShadow_Instanced() { return pointLightShadow_instanced; }
    static Shader* getDirectLightShadow_Instanced() { return directLightShadow_instanced; }

    // static Shader* getLineShader() { return lineShader; }
    // static Shader* getParticleShader() { return particleShader; }

    // static Shader* get(shaderNames shader);
    static Shader* get(gfx::shader::special shader);

    static Shader& get3D(const gfx::shader::shaders index);
    static Shader& get2D(const gfx::shader::shaders index);
    static Shader& getUtil(const gfx::shader::shadersUtil index);
    
    static uint32_t totat() { return SHADER_COUNT; }
};