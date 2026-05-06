#include <shader.h>
#include <renderer.h>

#include <iostream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::filesystem::path base = std::filesystem::current_path();

// ------------------------------ Class Functions ------------------------------------ //

Shader::Shader(const char* vertPath, const char* fragPath) {
    shaderProgram = Renderer::createShader(vertPath, fragPath);
}

Shader::Shader(const char* vertPath, const char* geomPath, const char* fragPath) {
    shaderProgram = Renderer::createShader2(vertPath, geomPath, fragPath);
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

void Renderer::setBool(const unsigned int& shaderProgram, const char* target, const bool& value) {

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void Renderer::setInt(const unsigned int &shaderProgram, const char* target, const int &value){

    glUniform1i(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void Renderer::setFloat(const unsigned int &shaderProgram, const char* target, const float &value){

    glUniform1f(
        glGetUniformLocation(shaderProgram, target),
        value
    );
}

void Renderer::setVec3(const unsigned int &shaderProgram, const char* target, const glm::vec3 &vector){

    glUniform3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        glm::value_ptr(vector)
    );
}

void Renderer::setMat3(const unsigned int &shaderProgram, const char* target, const glm::mat3 &matrix){

    glUniformMatrix3fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}

void Renderer::setMat4(const unsigned int &shaderProgram, const char* target, const glm::mat4 &matrix){

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, target),
        1,
        GL_FALSE,
        glm::value_ptr(matrix)
    );
}