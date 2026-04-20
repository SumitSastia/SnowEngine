#include <shader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::filesystem::path base = std::filesystem::current_path();

/*
Returns content of a Shader file in string format.
NOTE: This was created as a part of createShader, suggested to be not used externally.
*/
std::string loadShaderFile(const char* path) {

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
Returns a Complete Shader with Pipeline
-> Vertex -> Fragment ->
*/
const unsigned int createShader(const char* vertPath, const char* fragPath) {

    // Vertex & Fragment Shader //

    std::string vertexStr = loadShaderFile(vertPath);
    std::string fragmentStr = loadShaderFile(fragPath);

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
        return 0;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    // Shader Program //
    const unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/*
Returns a Complete Shader with Pipeline
-> Vertex -> Geometry -> Fragment ->
*/
const unsigned int createShader2(const char* vertPath, const char* geomPath, const char* fragPath) {

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
        return 0;
    }

    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, nullptr, infoLog);
        std::cout << "ERROR: GEOMETRY-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: FRAGMENT-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    // Shader Program //
    const unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: SHADER-PROGRAM LINKING FAILED!\n" << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/*
Renders the Object (vertices & indices).
NOTE: This method is used in rendering the shadow of the object.
*/
void renderShadow(const u_int& VAO, const u_int& indicesCount) {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
}

/*
Renders the Object (vertices & indices),
instanceCounts = No. of instances of the object to render.
NOTE: This method is used in rendering the shadow of the object.
*/
void renderShadowInstanced(const u_int& VAO, const u_int& indicesCount, const u_int& instanceCounts){

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, instanceCounts);
    glBindVertexArray(0);
}

// ------------------------------ Class Functions ------------------------------------ //

/*
Loads the image and allocate it into the Memory.
NOTE: "path" should start with '/' and rest should continue after the Main Directory (/SnowEngine).
*/
void texture2D::load(const char* path){

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

void texture2D::destroy() {

}

// ------------------------------ Shader Uniform Setter ------------------------------ //

void setBool(const unsigned int& shaderProgram, const char* target, const bool& value) {

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void setInt(const unsigned int &shaderProgram, const char* target, const int &value){

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void setFloat(const unsigned int &shaderProgram, const char* target, const float &value){

    glUniform1f(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void setVec3(const unsigned int &shaderProgram, const char* target, const glm::vec3 &vector){

    glUniform3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        glm::value_ptr(vector)
    );
}

void setMat3(const unsigned int &shaderProgram, const char* target, const glm::mat3 &matrix){

    glUniformMatrix3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

void setMat4(const unsigned int &shaderProgram, const char* target, const glm::mat4 &matrix){

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}