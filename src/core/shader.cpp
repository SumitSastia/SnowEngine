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

// Shader::Shader(const std::string vertexStr, const std::string fragmentStr) {

//     // Vertex & Fragment Shader //

//     const char* vertexShaderSource = vertexStr.c_str();
//     const char* fragmentShaderSource = fragmentStr.c_str();

//     const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

//     int success;
//     char infoLog[512];

//     glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//     glCompileShader(vertexShader);

//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//     glCompileShader(fragmentShader);

//     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

//     if (!success) {
//         glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
//         std::cout << "ERROR: VERTEX-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
//         // std::cout << "File: " << vertexStr << std::endl;
//     }

//     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

//     if (!success) {
//         glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
//         std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
//         // std::cout << "File: " << fragmentStr << std::endl;
//     }

//     // Shader Program //
//     shaderProgram = glCreateProgram();

//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);

//     glLinkProgram(shaderProgram);
//     glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

//     if (!success) {
//         glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
//         std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
//     }

//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);
// }

// Shader::Shader(const char* vertPath, const char* geomPath, const char* fragPath) {
    
//     // Vertex & Fragment Shader //

//     std::string vertexStr = loadShaderFile(vertPath);
//     std::string geometryStr = loadShaderFile(geomPath);
//     std::string fragmentStr = loadShaderFile(fragPath);

//     const char* vertexShaderSource = vertexStr.c_str();
//     const char* geometryShaderSource = geometryStr.c_str();
//     const char* fragmentShaderSource = fragmentStr.c_str();

//     const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     const unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
//     const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

//     int success;
//     char infoLog[512];

//     glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//     glCompileShader(vertexShader);

//     glShaderSource(geometryShader, 1, &geometryShaderSource, nullptr);
//     glCompileShader(geometryShader);

//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//     glCompileShader(fragmentShader);

//     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

//     if (!success) {
//         glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
//         std::cout << "ERROR: VERTEX-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
//     }

//     glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

//     if (!success) {
//         glGetShaderInfoLog(geometryShader, 512, nullptr, infoLog);
//         std::cout << "ERROR: GEOMETRY-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
//     }

//     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

//     if (!success) {
//         glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
//         std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
//     }

//     // Shader Program //
//     shaderProgram = glCreateProgram();

//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, geometryShader);
//     glAttachShader(shaderProgram, fragmentShader);

//     glLinkProgram(shaderProgram);
//     glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

//     if (!success) {
//         glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
//         std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
//     }

//     glDeleteShader(vertexShader);
//     glDeleteShader(geometryShader);
//     glDeleteShader(fragmentShader);
// }

void Shader::use() const {
    glUseProgram(shaderProgram);
}

void Shader::destroy() {
    
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;
}

// ------------------------------ Shaders -------------------------------------------- //

std::vector <bool>    ShaderManager::isLoaded (MAX_SHADERS, false);
std::vector <Shader*> ShaderManager::loadedShaders (MAX_SHADERS);

std::vector <Shader>  ShaderManager::lShaders3D   (MAX_SHADERS);
std::vector <Shader>  ShaderManager::lShaders2D   (MAX_SHADERS);
std::vector <Shader>  ShaderManager::lShadersUtil (MAX_SHADERS);

std::vector <Shader*> ShaderManager::specialShaders(gfx::shader::TOTAL_SPECIAL_SHADERS);

Shader* ShaderManager::pointLightShadow  = nullptr;
Shader* ShaderManager::directLightShadow = nullptr;

Shader* ShaderManager::pointLightShadow_instanced  = nullptr;
Shader* ShaderManager::directLightShadow_instanced = nullptr;

// Shader* ShaderManager::lineShader     = nullptr;
// Shader* ShaderManager::particleShader = nullptr;

std::vector <ShaderPath> ShaderManager::path = {

    ShaderPath("ecs/generic3d.vert",        "ecs/test.frag"),
    ShaderPath("ecs/generic2d.vert",        "ecs/test.frag"),
    ShaderPath("lights/ecs.vert",           "lights/light.frag"),
    ShaderPath("ecs/generic3d.vert",        "ecs/albedo.frag"),
    ShaderPath("ecs/generic2d.vert",        "ecs/albedo.frag"),
    ShaderPath("ecs/generic3d.vert",        "ecs/phong.frag", true),
    ShaderPath("ecs/generic2d.vert",        "ecs/phong.frag", true),
    ShaderPath("ecs/instance3d.vert",       "ecs/phong.frag", true),
    ShaderPath("ecs/instance2d.vert",       "ecs/phong.frag", true),
    ShaderPath("ecs/norm3d.vert",           "ecs/normPhong.frag", true),
    ShaderPath("ecs/norm2d.vert",           "ecs/normPhong.frag", true),
    ShaderPath("ecs/p2d.vert",              "ecs/p2d.frag", true),
    ShaderPath("ecs/generic3d.vert",        "ecs/pbr.frag", true),
    ShaderPath("ecs/generic2d.vert",        "ecs/pbr.frag", true),
    ShaderPath("ecs/generic3d.vert",        "ecs/pbrColor.frag", true),
    ShaderPath("ecs/generic2d.vert",        "ecs/pbrColor.frag", true),
    ShaderPath("ecs/norm3d.vert",           "ecs/normPbr.frag", true),
    ShaderPath("ecs/norm2d.vert",           "ecs/normPbr.frag", true),
    ShaderPath("ecs/instance3d.vert",       "ecs/pbr.frag", true),
    ShaderPath("ecs/instance2d.vert",       "ecs/pbr.frag", true),
    ShaderPath("cubeMap/env.vert",          "cubeMap/env.frag"),
    ShaderPath("deferred/gbuffer3d.vert",   "deferred/ecs_gbuffer.frag"),
    ShaderPath("deferred/gbuffer2d.vert",   "deferred/ecs_gbuffer.frag"),
    ShaderPath("deferred/gbufferInst.vert", "deferred/ecs_gbuffer.frag"),
    
    ShaderPath("deferred/gbuffer_normalTex3d.vert",   "deferred/gbuffer_normalTex.frag"),
    ShaderPath("deferred/gbuffer_normalTex2d.vert",   "deferred/gbuffer_normalTex.frag"),

    ShaderPath("wireframe/generic3d.vert", "wireframe/wireframe.frag"),
    ShaderPath("ecs/norm2d.vert",           "ecs/normPbr2d_bf.frag", true)
};

std::vector <ShaderPath> ShaderManager::path3D = {

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

    ShaderPath("ShaderPath Not Initialized", "ShaderPath Not Initialized")
};

std::vector <ShaderPath> ShaderManager::path2D = {

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

    ShaderPath("ecs/p2d.vert",              "ecs/p2d.frag", true),
};

std::vector <ShaderPath> ShaderManager::pathUtil = {

    ShaderPath("line/line.vert",           "line/line.frag"),
    ShaderPath("lights/ecs.vert",          "lights/light.frag"),
    ShaderPath("wireframe/generic3d.vert", "wireframe/wireframe.frag"),
    ShaderPath("cubeMap/env.vert",         "cubeMap/env.frag"),
    ShaderPath("obj2d/particle.vert",      "obj2d/particle.frag"),
    ShaderPath("obj2d/tex_particle.vert",  "obj2d/tex_particle.frag")
};

bool ShaderManager::initShaders() {

    pointLightShadow = new Shader();
    pointLightShadow_instanced = new Shader();
    directLightShadow = new Shader();
    directLightShadow_instanced = new Shader();

    pointLightShadow->loadFromFile(
        "../shaders/pointShadow/shadow.vert",
        "../shaders/pointShadow/shadow.geom",
        "../shaders/pointShadow/shadow.frag"
    );

    pointLightShadow_instanced->loadFromFile(
        "../shaders/pointShadow/instancedShadow.vert",
        "../shaders/pointShadow/shadow.geom",
        "../shaders/pointShadow/shadow.frag"
    );

    directLightShadow->loadFromFile(
        "../shaders/directShadow/directShadow.vert",
        "../shaders/directShadow/directShadow.frag"
    );

    directLightShadow_instanced->loadFromFile(
        "../shaders/directShadow/directShadowInstanced.vert",
        "../shaders/directShadow/directShadow.frag"
    );

    // lineShader = new Shader(
    //     "../shaders/line/line.vert",
    //     "../shaders/line/line.frag"
    // );

    // particleShader = new Shader(
    //     "../shaders/obj2d/tex_particle.vert",
    //     "../shaders/obj2d/tex_particle.frag"
    // );

    // specialShaders = {
    //     new Shader(
    //         "../shaders/obj2d/particle.vert",
    //         "../shaders/obj2d/particle.frag"
    //     ),
    //     particleShader
    // };

    return true;
}

// Shader* ShaderManager::get(shaderNames shader) {

//     if (!isLoaded[shader]) {

//         const std::string base_path = "../shaders/";

//         const std::string& vertPath = base_path + path[shader].vert;
//         const std::string& fragPath = base_path + path[shader].frag;

//         // loadedShaders[shader] = new Shader(vertPath.c_str(), fragPath.c_str(), path[shader].preprocess);
//         isLoaded[shader] = true;
//     }

//     return loadedShaders[shader];
// }

Shader* ShaderManager::get(gfx::shader::special shader) {

    if (shader >= gfx::shader::TOTAL_SPECIAL_SHADERS) return nullptr;
    return specialShaders[shader];
}

Shader& ShaderManager::get3D(const gfx::shader::shaders index) {

    if (!lShaders3D[index].getShader()) {

        const std::string base_path = "../shaders/";

        const std::string& vertPath = base_path + path3D[index].vert;
        const std::string& fragPath = base_path + path3D[index].frag;

        lShaders3D[index].loadFromFile(vertPath.c_str(), fragPath.c_str(), path3D[index].preprocess);
    }

    return lShaders3D[index];
}

Shader& ShaderManager::get2D(const gfx::shader::shaders index) {

    if (!lShaders2D[index].getShader()) {

        const std::string base_path = "../shaders/";

        const std::string& vertPath = base_path + path2D[index].vert;
        const std::string& fragPath = base_path + path2D[index].frag;

        lShaders2D[index].loadFromFile(vertPath.c_str(), fragPath.c_str(), path2D[index].preprocess);
    }

    return lShaders2D[index];
}

Shader& ShaderManager::getUtil(const gfx::shader::shadersUtil index) {

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
