#include <shader.h>
#include <renderer.h>
#include <lights.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::filesystem::path base = std::filesystem::current_path();

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
            std::filesystem::path headerPath = std::filesystem::path(path).parent_path() / ".." / headerName;

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
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
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

void Texture2D::load(const char* path){

    std::string path_str(path);
    std::string base_str = base.string();

    base_str.erase(base_str.size() - 5);

    std::string finalPath = base_str + path_str;

    pixelData = stbi_load(finalPath.c_str(), &width, &height, nullptr, 4);

    if(!pixelData){
        std::cerr << "Failed to Load Image!\n" << finalPath << std::endl;
        return;
    }

    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::destroy() {

    glDeleteTextures(1, &textureID);
    textureID = 0;
}

// ------------------------------ Shader Uniform Setter ------------------------------ //

void Shader::setBool(const char* target, const bool& value) const {

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
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
