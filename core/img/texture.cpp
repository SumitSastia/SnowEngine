#include <stb_image.h>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "texture.h"

#include "utils/debug.h"
#include "utils/assert.h"

#include "core/shader.h"
#include "core/camera.h"

void Texture::load(const uint32_t color, const bool isRGBA) {

    width  = 1;
    height = 1;

    pixelData = new unsigned char[4];

    pixelData[0] = (color >> 24) & 0xFF;
    pixelData[1] = (color >> 16) & 0xFF;
    pixelData[2] = (color >> 8)  & 0xFF;
    pixelData[3] = color & 0xFF;

    this->isRGBA = isRGBA;
}

void Texture::load(const std::string& path, const bool isRGBA) {

    // If .exe is run through build directory
    const std::string finalPath = "../" + path;

    // pixelData = stbi_load(path, &width, &height, nullptr, 4);
    pixelData = stbi_load(finalPath.c_str(), &width, &height, nullptr, 4);

    if (!pixelData) {
        // std::cerr << "ERROR::FAILED TO OPEN TEXTURE FILE!\nFILE: " << path << '\n';
        std::cerr << "ERROR::FAILED TO OPEN TEXTURE FILE!\nFILE: " << finalPath << '\n';
        return;
    }

    this->isRGBA = isRGBA;
}

void Texture::compile(bool generateMipMaps) {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // const float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, (isRGBA)? GL_RGBA32F : GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    if (generateMipMaps) glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (width > 1 && height > 1) {
        stbi_image_free(pixelData);
    }
    else {
        delete pixelData;
    }

    pixelData = nullptr;
}

void Texture::bind(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

///////////////////////////////////////////////////////////////

GLuint Line::VAO = 0;
GLuint Line::VBO = 0;

void Line::init() {

    glm::vec3 vertices[2] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f }
    };

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Line::renderHelper(
    const glm::mat4& model,
    const glm::vec3& color
) {
    // const Shader& shader = *ShaderManager::getLineShader();
    const Shader& shader = ShaderManager::getUtil(gfx::shader::LINE);

    shader.use();

    shader.setMat4(
        "finalMatrix", 
        Camera::activeCamera->getProjection() * Camera::activeCamera->getView() * model
        // model
    );

    shader.setVec3("color", color);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
}

void Line::render(
    const glm::vec3& point1,
    const glm::vec3& point2,
    const glm::vec3& color
) {
    glm::vec3 direction = point2 - point1;
    float length = glm::length(direction);

    const glm::vec3 xAxis = glm::normalize(direction);

    glm::quat rotation = glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), xAxis);

    glm::mat4 modelMatrix { 1.0f };
    
    modelMatrix  = glm::translate(modelMatrix, point1);
    modelMatrix *= glm::mat4_cast(rotation);
    modelMatrix  = glm::scale(modelMatrix, glm::vec3(length, 1.0f, 1.0f));

    renderHelper(modelMatrix, color);
}

void Line::renderDirection(
    const glm::vec3& point,
    const glm::vec3& direction,
    const float length,
    const glm::vec3& color
) {
    // const glm::vec3& point2  = point + glm::normalize(direction) * length;
    const glm::quat rotation = glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::normalize(direction));

    glm::mat4 modelMatrix { 1.0f };
    
    modelMatrix  = glm::translate(modelMatrix, point);
    modelMatrix *= glm::mat4_cast(rotation);
    modelMatrix  = glm::scale(modelMatrix, glm::vec3(length, 1.0f, 1.0f));

    renderHelper(modelMatrix, color);
}

//-------------------------------------------------------------------------------------//