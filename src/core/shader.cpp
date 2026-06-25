#include <shader.h>
#include <renderer.h>
#include <lights.h>
#include <debug.h>
#include <ibl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::filesystem::path base = std::filesystem::current_path();

// ------------------------------ Foward Declarations -------------------------------- //

namespace gfx::cubemap {

    unsigned int Cube::vao = 0;
    unsigned int Cube::vbo = 0;

    Cube::Cube() {

        const float cubemapVertices[] = {
            
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

};

// ------------------------------ Class Functions ------------------------------------ //

/*
@return Content of a Shader file in string format.
@note This was created as a part of createShader, suggested not to be used externally.
*/
std::string Shader::loadShaderFile(const char* path) {

    std::ifstream file(path);

    if (!file) {
        std::cerr << "Failed to open the File!" << std::endl;
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

Shader::Shader(const std::string vertexStr, const std::string fragmentStr) {

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

Shader::Shader(const char* vertPath, const char* geomPath, const char* fragPath) {
    
    // Vertex & Fragment Shader //

    std::string vertexStr = loadShaderFile(vertPath);
    std::string geometryStr = loadShaderFile(geomPath);
    std::string fragmentStr = loadShaderFile(fragPath);

    const char* vertexShaderSource = vertexStr.c_str();
    const char* geometryShaderSource = geometryStr.c_str();
    const char* fragmentShaderSource = fragmentStr.c_str();

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

void Shader::use() const {
    glUseProgram(shaderProgram);
}

void Shader::destroy() {
    
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;
}

void Texture2D::load(const char* path, const bool format) {

    // running::core::timer t;

    std::string path_str(path);
    std::string base_str = base.string();

    base_str.erase(base_str.size() - 5);

    std::string finalPath = base_str + path_str;

    unsigned char* pixelData = stbi_load(finalPath.c_str(), &width, &height, nullptr, 4);

    if(!pixelData){
        std::cerr << "Failed to Load Image!\n" << finalPath << std::endl;
        return;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, (format)? GL_SRGB : GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // DebugMenu::log("Image -> Memory Bind: " + t.end());

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);

    // DebugMenu::log("Cleaning stbi memory: " + t.end());
}

void Texture2D::bind(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture2D::destroy() {

    glDeleteTextures(1, &textureID);

    textureID = 0;
    width = 0; height = 0;
}

// ------------------------------ Shaders -------------------------------------------- //

std::vector <bool>    Shaders::isLoaded(MAX_SHADERS, false);
std::vector <Shader*> Shaders::loadedShaders(MAX_SHADERS);

Shader* Shaders::pointLightShadow  = nullptr;
Shader* Shaders::directLightShadow = nullptr;

Shader* Shaders::pointLightShadow_instanced  = nullptr;
Shader* Shaders::directLightShadow_instanced = nullptr;

Shader* Shaders::lineShader = nullptr;

std::vector <shader_paths> Shaders::path = {

    shader_paths("ecs/generic3d.vert",        "ecs/test.frag"),
    shader_paths("ecs/generic2d.vert",        "ecs/test.frag"),
    shader_paths("lights/ecs.vert",           "lights/light.frag"),
    shader_paths("ecs/generic3d.vert",        "ecs/albedo.frag"),
    shader_paths("ecs/generic2d.vert",        "ecs/albedo.frag"),
    shader_paths("ecs/generic3d.vert",        "ecs/phong.frag", true),
    shader_paths("ecs/generic2d.vert",        "ecs/phong.frag", true),
    shader_paths("ecs/instance3d.vert",       "ecs/phong.frag", true),
    shader_paths("ecs/instance2d.vert",       "ecs/phong.frag", true),
    shader_paths("ecs/norm3d.vert",           "ecs/normPhong.frag", true),
    shader_paths("ecs/norm2d.vert",           "ecs/normPhong.frag", true),
    shader_paths("ecs/p2d.vert",              "ecs/p2d.frag", true),
    shader_paths("ecs/generic3d.vert",        "ecs/pbr.frag", true),
    shader_paths("ecs/generic2d.vert",        "ecs/pbr.frag", true),
    shader_paths("ecs/generic3d.vert",        "ecs/pbrColor.frag", true),
    shader_paths("ecs/generic2d.vert",        "ecs/pbrColor.frag", true),
    shader_paths("ecs/norm3d.vert",           "ecs/normPbr.frag", true),
    shader_paths("ecs/norm2d.vert",           "ecs/normPbr.frag", true),
    shader_paths("ecs/instance3d.vert",       "ecs/pbr.frag", true),
    shader_paths("ecs/instance2d.vert",       "ecs/pbr.frag", true),
    shader_paths("cubeMap/env.vert",          "cubeMap/env.frag"),
    shader_paths("deferred/gbuffer3d.vert",   "deferred/ecs_gbuffer.frag"),
    shader_paths("deferred/gbuffer2d.vert",   "deferred/ecs_gbuffer.frag"),
    shader_paths("deferred/gbufferInst.vert", "deferred/ecs_gbuffer.frag"),
    
    shader_paths("deferred/gbuffer_normalTex3d.vert",   "deferred/gbuffer_normalTex.frag"),
    shader_paths("deferred/gbuffer_normalTex2d.vert",   "deferred/gbuffer_normalTex.frag"),

    shader_paths("wireframe/generic3d.vert", "wireframe/wireframe.frag"),
    shader_paths("ecs/norm2d.vert",           "ecs/normPbr2d_bf.frag", true)
};

bool Shaders::initShaders() {

    pointLightShadow = new Shader(
        "../shaders/pointShadow/shadow.vert",
        "../shaders/pointShadow/shadow.geom",
        "../shaders/pointShadow/shadow.frag"
    );

    pointLightShadow_instanced = new Shader(
        "../shaders/pointShadow/instancedShadow.vert",
        "../shaders/pointShadow/shadow.geom",
        "../shaders/pointShadow/shadow.frag"
    );

    directLightShadow = new Shader(
        "../shaders/directShadow/directShadow.vert",
        "../shaders/directShadow/directShadow.frag"
    );

    directLightShadow_instanced = new Shader(
        "../shaders/directShadow/directShadowInstanced.vert",
        "../shaders/directShadow/directShadow.frag"
    );

    lineShader = new Shader(
        "../shaders/line/line.vert",
        "../shaders/line/line.frag"
    );

    return true;
}

Shader* Shaders::get(shaderNames shader) {

    if (!isLoaded[shader]) {

        const std::string base_path = "../shaders/";

        const std::string& vertPath = base_path + path[shader].vert;
        const std::string& fragPath = base_path + path[shader].frag;

        loadedShaders[shader] = new Shader(vertPath.c_str(), fragPath.c_str(), path[shader].preprocess);
        isLoaded[shader] = true;
    }

    return loadedShaders[shader];
}

// ------------------------------ Shaders -------------------------------------------- //

CubeMap::CubeMap(const std::vector <std::string>& textureFaces, const uint16_t internal_format, const uint16_t type, const uint16_t res_size) {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    unsigned char* pixelData;
    int width, height, nrChannels;

    const unsigned int size = textureFaces.size();

    std::string base_str = base.string();
    base_str.erase(base_str.size() - 5);

    for (unsigned int i = 0; i < size; i++) {

        std::string finalPath = base_str + textureFaces[i];

        pixelData = stbi_load(
            finalPath.c_str(),
            &width, &height,
            &nrChannels, 0
        );

        if (pixelData) {

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, internal_format, (res_size)? res_size : width, (res_size)? res_size : height,
                0, GL_RGB, type, pixelData
            );
        }
        else {
            std::cerr << "ERROR::FAILED TO LOAD CUBEMAP!" << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::bindTexture(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void CubeMap::destroy() {

    glDeleteTextures(1, &textureID);
}

Skybox::Skybox(const std::vector <std::string>& textureFaces) {

    _cubeMap  = new CubeMap(textureFaces);
    isVisible = false;

    lightIntensity = 0.8f;
    _irradianceMap = nullptr;
}

void Skybox::setIrradianceMap(const std::vector <std::string>& textureFaces) {

    if (_irradianceMap) {
    
        _irradianceMap->destroy();
        delete _irradianceMap;
    }

    _irradianceMap = new CubeMap(textureFaces);
}

void Skybox::bindTexture(const unsigned int textureUnit) const {
   _cubeMap->bindTexture(textureUnit);
}

void Skybox::bindIrradiance(const unsigned int textureUnit) const {
    _irradianceMap->bindTexture(textureUnit);
}

void Skybox::draw() const {

    glBindVertexArray(gfx::cubemap::Cube::getVAO());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Skybox::destroy() {
    delete _cubeMap;
}

Environment::Environment(const char* path, const uint16_t resolution) {

    m_cubeMap = nullptr;
    isVisible = true;
    m_irradianceMap = nullptr;

    iblFrame = new IBLFrame(path, resolution);
}

Environment::Environment(const std::vector <std::string>& textureFaces) {

    m_cubeMap  = new CubeMap(textureFaces);
    isVisible  = false;

    // Not auto-generated currently
    m_irradianceMap = nullptr;
    iblFrame        = nullptr;
}

void Environment::setIrradianceMap(const std::vector <std::string>& textureFaces) {

    if (m_irradianceMap) {
    
        m_irradianceMap->destroy();
        delete m_irradianceMap;
    }

    m_irradianceMap = new CubeMap(textureFaces);
}

void Environment::bindTexture(const unsigned int textureUnit) const {
   
   if (m_cubeMap) m_cubeMap->bindTexture(textureUnit);
   if (iblFrame) iblFrame->bindEnv(textureUnit);
}

void Environment::bindIrradiance(const unsigned int textureUnit) const {
    if (m_irradianceMap) m_irradianceMap->bindTexture(textureUnit);
    if (iblFrame) iblFrame->bindIrradianceMap(textureUnit);
}

void Environment::bindPrefilter(const unsigned int textureUnit) const {
    iblFrame->bindPreFilterMap(textureUnit);
}

void Environment::bindBRDF(const unsigned int textureUnit) const {
    iblFrame->bindBRDFLUT(textureUnit);
}

void Environment::draw() const {

    glBindVertexArray(gfx::cubemap::Cube::getVAO());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Environment::destroy() {
    delete m_cubeMap;
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

void Shader::setVec3(const char* target, const glm::vec3 &vector) const {

    glUniform3fv(
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
