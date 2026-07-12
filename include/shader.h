#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <core/config.h>

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
    void setVec2 (const char* target, const glm::vec2& vector) const;
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

namespace gfx::shader {

    const uint8_t PARTICLE_SHADER_OFFSET = 5;

    enum shaders : ShaderHandle {

        S3D_LIGHT,
        S3D_ALBEDO,
        S3D_PHONG,
        S3D_PHONG_NORM,
        S3D_PHONG_INSTANCE,
        S3D_PBR,
        S3D_PBR_NORM,
        S3D_PBR_INSTANCE,
        S3D_GBUFFER,
        S3D_GBUFFER_NORM,
        S3D_GBUFFER_INSTANCE,
        S3D_PARALLAX,

        S2D_LIGHT,
        S2D_ALBEDO,
        S2D_PHONG,
        S2D_PHONG_NORM,
        S2D_PHONG_INSTANCE,
        S2D_PBR,
        S2D_PBR_NORM,
        S2D_PBR_INSTANCE,
        S2D_GBUFFER,
        S2D_GBUFFER_NORM,
        S2D_GBUFFER_INSTANCE,
        S2D_PARALLAX,

        SHADER_TOTAL_COUNT
    };

    enum shadersUtil : uint8_t {
        
        TEXT,
        LINE,
        LIGHT,
        WIREFRAME,
        ENVIRONMENT,
        PARTICLE_HARD_COLORED,
        PARTICLE_HARD_TEXTURED,
        PARTICLE_SOFT_COLORED,
        PARTICLE_SOFT_TEXTURED,
        PARTICLE_INSTANCED_HARD_COLORED,
        PARTICLE_INSTANCED_HARD_TEXTURED,
        PARTICLE_INSTANCED_SOFT_COLORED,
        PARTICLE_INSTANCED_SOFT_TEXTURED,

        SHADERUTIL_TOTAL_COUNT
    };

    enum shadersFrame : uint8_t {

        FRAME_DEFAULT,
        FRAME_HDR,
        FRAME_BLOOM,
        FRAME_BLUR,
        FRAME_DEPTH,
        
        EQUIRECT_TO_CUBEMAP,
        BLUR_CUBEMAP,
        PREFILTER_CUBEMAP,
        BRDF,
        SSAO,
        SSAO_BLUR,
        DEFERRED_LIGHTNING,

        SHADERFRAME_TOTAL_COUNT
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

    static Shader pointLightShadow;
    static Shader directLightShadow;

    static Shader pointLightShadow_instanced;
    static Shader directLightShadow_instanced;

    static std::vector <ShaderPath> paths;
    static std::vector <ShaderPath> pathUtil;

    static std::vector <Shader> objShaders;
    static std::vector <Shader> lShadersUtil;
    
    //-------------------------------------------------------------//

    static std::vector <Shader> shaderFrames;

public:

    static bool initShaders();

    static const Shader& getPointLightShadow() { return pointLightShadow; }
    static const Shader& getDirectLightShadow() { return directLightShadow; }

    static const Shader& getPointLightShadow_Instanced() { return pointLightShadow_instanced; }
    static const Shader& getDirectLightShadow_Instanced() { return directLightShadow_instanced; }
    
    static const Shader& getShader(const ShaderHandle handle);

    static const Shader& getUtil(const uint8_t index);
    static const Shader& getFrame(const gfx::shader::shadersFrame index) { return shaderFrames[index]; }
    
    static uint32_t totat() { return gfx::shader::SHADER_TOTAL_COUNT; }
};