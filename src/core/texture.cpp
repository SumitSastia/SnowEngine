#include <texture.h>
#include <debug/assert.h>

#include <stb_image.h>
#include <glad/glad.h>

#include <filesystem>

void Texture::load(const uint32_t color) {

    width  = 1;
    height = 1;

    pixelData = new unsigned char[4];

    pixelData[0] = (color >> 24) & 0xFF;
    pixelData[1] = (color >> 16) & 0xFF;
    pixelData[2] = (color >> 8)  & 0xFF;
    pixelData[3] = color & 0xFF;

    this->isRGBA = true;
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

void Texture::compile() {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    const float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, (isRGBA)? GL_RGBA : GL_SRGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

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