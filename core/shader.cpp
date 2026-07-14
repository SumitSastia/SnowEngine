#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "core/renderer.h"
#include "core/lights.h"

#include "utils/debug.h"
#include "utils/assert.h"

std::filesystem::path base = std::filesystem::current_path();

// ------------------------------ Foward Declarations -------------------------------- //

void Shader::loadFromFile(const char* vertPath, const char* fragPath, const bool preprocess) {
    
    if (!preprocess) loadFromString(loadShaderFile(vertPath), loadShaderFile(fragPath));
    else             loadFromString(loadShaderFile(vertPath), preprocessFile(fragPath));
}

void Shader::loadFromFile(const char* vertPath, const char* geomPath, const char* fragPath) {
    loadFromString(loadShaderFile(vertPath), loadShaderFile(geomPath), loadShaderFile(fragPath));
}

void Shader::loadFromString(const std::string& vertexStr, const std::string& fragmentStr) {

    // Vertex & Fragment Shader //

    const char* vertexShaderSource = vertexStr.c_str();
    const char* fragmentShaderSource = fragmentStr.c_str();

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR: VERTEX-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        // std::cout << "File: " << vertexStr << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        // std::cout << "File: " << fragmentStr << std::endl;
    }

    // Shader Program //
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::loadFromString(const std::string& vertStr, const std::string& geomStr, const std::string& fragStr) {

    const char* vertexShaderSource   = vertStr.c_str();
    const char* geometryShaderSource = geomStr.c_str();
    const char* fragmentShaderSource = fragStr.c_str();

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glShaderSource(geometryShader, 1, &geometryShaderSource, nullptr);
    glCompileShader(geometryShader);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR: VERTEX-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
    }

    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, nullptr, infoLog);
        std::cout << "ERROR: GEOMETRY-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
    }

    // Shader Program //
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);
}

// ------------------------------ Class Functions ------------------------------------ //

/*
@return Content of a Shader file in string format.
@note This was created as a part of createShader, suggested not to be used externally.
*/
std::string Shader::loadShaderFile(const char* path) {

    std::ifstream file(path);

    if (!file) {
        std::cerr << "Failed to open the File!\n path: " << path << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();

    return ss.str();
}

/*
EXPERIMENTAL: 
Replaces "#include" inside file content with header file data.
@param path Directory in which headers are present.
@return Content of a Shader file in string format.
@note This was created as a part of createShader, suggested not to be used externally.
*/
std::string Shader::preprocessFile(const char* path) {

    std::string src_str = loadShaderFile(path);
    
    std::stringstream input(src_str);
    std::stringstream output;
    
    std::string line;
    
    // Replacing #include
    while (std::getline(input, line)) {
        
        if (line.find("#include") != std::string::npos) {
            
            size_t temp1 = line.find("<");
            size_t temp2 = line.find(">");

            // Extracting header name
            std::string headerName = line.substr(temp1 + 1, temp2 - temp1 - 1);

            // Header filePath
            std::filesystem::path headerPath = std::filesystem::weakly_canonical(std::filesystem::path(path).parent_path() / ".." / headerName);

            // std::cout << "Preprocessing: " << path << '\n';
            // std::cout << "Header: " << headerPath << std::endl;

            std::string header_str = loadShaderFile(headerPath.c_str());

            output << header_str << '\n';
        }
        else {
            output << line << '\n';
        }
    }

    return output.str();
}

void Shader::use() const {
    glUseProgram(shaderProgram);
}

void Shader::destroy() {
    
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;
}

// ------------------------------ Shaders -------------------------------------------- //

Shader ShaderManager::pointLightShadow  {};
Shader ShaderManager::directLightShadow {};

Shader ShaderManager::pointLightShadow_instanced  {};
Shader ShaderManager::directLightShadow_instanced {};

std::vector <Shader> ShaderManager::objShaders   (MAX_SHADERS);
std::vector <Shader> ShaderManager::lShadersUtil (MAX_SHADERS);

std::vector <Shader> ShaderManager::shaderFrames (gfx::shader::SHADERFRAME_TOTAL_COUNT);

std::vector <ShaderPath> ShaderManager::paths = {

    // 3D

    ShaderPath("lights/ecs.vert",          "lights/light.frag"),
    ShaderPath("ecs/generic3d.vert",        "ecs/albedo.frag"),

    ShaderPath("ecs/generic3d.vert",        "ecs/phong.frag", true),
    ShaderPath("ecs/norm3d.vert",           "ecs/normPhong.frag", true),
    ShaderPath("ecs/instance3d.vert",       "ecs/phong.frag", true),

    ShaderPath("ecs/generic3d.vert",        "ecs/pbr.frag", true),
    ShaderPath("ecs/norm3d.vert",           "ecs/normPbr.frag", true),
    ShaderPath("ecs/instance3d.vert",       "ecs/pbr.frag", true),

    ShaderPath("deferred/gbuffer3d.vert",   "deferred/ecs_gbuffer.frag"),
    ShaderPath("deferred/gbuffer_normalTex3d.vert",   "deferred/gbuffer_normalTex.frag"),
    ShaderPath("deferred/gbufferInst.vert", "deferred/ecs_gbuffer.frag"),

    ShaderPath("ShaderPath Not Initialized", "ShaderPath Not Initialized"),

    // 2D

    ShaderPath("ShaderPath Not Initialized", "ShaderPath Not Initialized"),
    ShaderPath("ecs/generic2d.vert",        "ecs/albedo.frag"),

    ShaderPath("ecs/generic2d.vert",        "ecs/phong.frag", true),
    ShaderPath("ecs/norm2d.vert",           "ecs/normPhong.frag", true),
    ShaderPath("ecs/instance2d.vert",       "ecs/phong.frag", true),

    ShaderPath("ecs/generic2d.vert",        "ecs/pbr.frag", true),
    ShaderPath("ecs/norm2d.vert",           "ecs/normPbr.frag", true),
    ShaderPath("ecs/instance2d.vert",       "ecs/pbr.frag", true),

    ShaderPath("deferred/gbuffer2d.vert",   "deferred/ecs_gbuffer.frag"),
    ShaderPath("deferred/gbuffer_normalTex2d.vert",   "deferred/gbuffer_normalTex.frag"),
    ShaderPath("ShaderPath Not Initialized", "ShaderPath Not Initialized"),

    ShaderPath("ecs/p2d.vert",              "ecs/p2d.frag", true)
};

std::vector <ShaderPath> ShaderManager::pathUtil = {

    ShaderPath("text/text.vert",           "text/text.frag"),
    ShaderPath("line/line.vert",           "line/line.frag"),
    ShaderPath("lights/ecs.vert",          "lights/light.frag"),
    ShaderPath("wireframe/generic3d.vert", "wireframe/wireframe.frag"),
    ShaderPath("cubeMap/env.vert",         "cubeMap/env.frag"),
    ShaderPath("obj2d/particle.vert",      "obj2d/particle_colored.frag"),
    ShaderPath("obj2d/particle.vert",      "obj2d/particle_textured.frag"),
    ShaderPath("obj2d/particle.vert",      "obj2d/particle_colored_soft.frag"),
    ShaderPath("obj2d/particle.vert",      "obj2d/particle_textured_soft.frag"),
    ShaderPath("obj2d/instance.vert",      "obj2d/particle_colored.frag"),
    ShaderPath("obj2d/instance.vert",      "obj2d/particle_textured.frag"),
    ShaderPath("obj2d/instance.vert",      "obj2d/particle_colored_soft.frag"),
    ShaderPath("obj2d/instance.vert",      "obj2d/particle_textured_soft.frag"),
};

bool ShaderManager::initShaders() {

    pointLightShadow.loadFromFile(
        "../shaders/pointShadow/shadow.vert",
        "../shaders/pointShadow/shadow.geom",
        "../shaders/pointShadow/shadow.frag"
    );

    pointLightShadow_instanced.loadFromFile(
        "../shaders/pointShadow/instancedShadow.vert",
        "../shaders/pointShadow/shadow.geom",
        "../shaders/pointShadow/shadow.frag"
    );

    directLightShadow.loadFromFile(
        "../shaders/directShadow/directShadow.vert",
        "../shaders/directShadow/directShadow.frag"
    );

    directLightShadow_instanced.loadFromFile(
        "../shaders/directShadow/directShadowInstanced.vert",
        "../shaders/directShadow/directShadow.frag"
    );

    // ------------ Shaders for FrameBuffers ------------ //

    shaderFrames[gfx::shader::FRAME_DEFAULT].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/default_fb.frag"
    );

    shaderFrames[gfx::shader::FRAME_HDR].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/hdr_frame.frag"
    );

    shaderFrames[gfx::shader::FRAME_BLOOM].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/bloom.frag"
    );

    shaderFrames[gfx::shader::FRAME_BLUR].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/blur.frag"
    );

    shaderFrames[gfx::shader::FRAME_DEPTH].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/depth.frag"
    );

    shaderFrames[gfx::shader::EQUIRECT_TO_CUBEMAP].loadFromFile(
        "../shaders/cubeMap/rect2cube.vert",
        "../shaders/cubeMap/rect2cube.frag"
    );

    shaderFrames[gfx::shader::BLUR_CUBEMAP].loadFromFile(
        "../shaders/cubeMap/env.vert",
        "../shaders/cubeMap/convolution.frag"
    );

    shaderFrames[gfx::shader::PREFILTER_CUBEMAP].loadFromFile(
        "../shaders/cubeMap/env.vert",
        "../shaders/cubeMap/prefilter.frag"
    );

    shaderFrames[gfx::shader::BRDF].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/cubeMap/brdf.frag"
    );

    shaderFrames[gfx::shader::SSAO].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/ssao/ssao.frag"
    );

    shaderFrames[gfx::shader::SSAO_BLUR].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/ssao/ssaoBlur.frag"
    );

    shaderFrames[gfx::shader::DEFERRED_LIGHTNING].loadFromFile(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/deferred.frag", true
    );

    return true;
}

const Shader& ShaderManager::getShader(const ShaderHandle handle) {

    SNOW_ASSERT(
        handle < gfx::shader::SHADER_TOTAL_COUNT,
        "SHADER ACCESSED OUTSIDE RANGE!"
    );

    if (!objShaders[handle].getShader()) {

        const std::string base_path = "../shaders/";

        const std::string& vertPath = base_path + paths[handle].vert;
        const std::string& fragPath = base_path + paths[handle].frag;

        objShaders[handle].loadFromFile(vertPath.c_str(), fragPath.c_str(), paths[handle].preprocess);
    }

    return objShaders[handle];
}

const Shader& ShaderManager::getUtil(const uint8_t index) {

    SNOW_ASSERT(
        index < gfx::shader::SHADERUTIL_TOTAL_COUNT,
        "SHADER-UTIL ACCESSED OUTSIDE RANGE!"
    );

    if (!lShadersUtil[index].getShader()) {

        const std::string base_path = "../shaders/";

        const std::string& vertPath = base_path + pathUtil[index].vert;
        const std::string& fragPath = base_path + pathUtil[index].frag;

        lShadersUtil[index].loadFromFile(vertPath.c_str(), fragPath.c_str(), pathUtil[index].preprocess);
    }

    return lShadersUtil[index];
}

// ------------------------------ Shader Uniform Setter ------------------------------ //

void Shader::setBool(const char* target, const bool value) const {

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        (int)value
    );
}

void Shader::setInt(const char* target, const int &value) const {

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void Shader::setFloat(const char* target, const float &value) const {

    glUniform1f(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void Shader::setVec2(const char* target, const glm::vec2 &vector) const {

    glUniform2fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        glm::value_ptr(vector)
    );
}

void Shader::setVec3(const char* target, const glm::vec3 &vector) const {

    glUniform3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        glm::value_ptr(vector)
    );
}

void Shader::setVec4(const char* target, const glm::vec4 &vector) const {

    glUniform4fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        glm::value_ptr(vector)
    );
}

void Shader::setMat3(const char* target, const glm::mat3 &matrix) const {

    glUniformMatrix3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

void Shader::setMat4(const char* target, const glm::mat4 &matrix) const {

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

void Shader::setPointLight(const std::string& target, const lights::PointLight& pl) const {

    this->setVec3((target + ".position").c_str(), pl.position);
    this->setVec3((target + ".color"   ).c_str(), pl.color);

    this->setFloat((target + ".constant" ).c_str(), pl.constant);
    this->setFloat((target + ".linear"   ).c_str(), pl.linear);
    this->setFloat((target + ".quadratic").c_str(), pl.quadratic);
}

void Shader::setPointLight(const std::uint32_t& target, const lights::PointLight& pl) const {

    const std::string id = std::to_string(target);

    // std::cout << ("pl[" + id + "].position").c_str() << std::endl;

    this->setVec3(("pl[" + id + "].position").c_str(), pl.position);
    this->setVec3(("pl[" + id + ".color"   ).c_str(), pl.color);

    this->setFloat(("pl[" + id + "].constant" ).c_str(), pl.constant);
    this->setFloat(("pl[" + id + "].linear"   ).c_str(), pl.linear);
    this->setFloat(("pl[" + id + "].quadratic").c_str(), pl.quadratic);
}

void Shader::setSpotLight(const std::string& target, const lights::SpotLight& sl) const {

    this->setVec3((target + ".position" ).c_str(), sl.position);
    this->setVec3((target + ".direction").c_str(), sl.direction);
    this->setVec3((target + ".color"    ).c_str(), sl.color);

    glUniform1i(glGetUniformLocation(shaderProgram, (target + ".isVisible").c_str()), sl.isVisible);

    this->setFloat((target + ".cutOffangle").c_str(), sl.cutOffangle);
    this->setFloat((target + ".outerCutOff").c_str(), sl.outerCutOff);

    this->setFloat((target + ".constant" ).c_str(), sl.constant);
    this->setFloat((target + ".linear"   ).c_str(), sl.linear);
    this->setFloat((target + ".quadratic").c_str(), sl.quadratic);
}

void Shader::setDirectionalLight(const std::string& target, const lights::DirectionalLight& dl) const {

    this->setVec3((target + ".direction").c_str(), dl.direction);
    this->setVec3((target + ".color"    ).c_str(), dl.color);
}
