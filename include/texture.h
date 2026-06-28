#pragma once

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace gfx::helper {

    inline const uint32_t rgba_to_hex(const glm::vec4& color) {

        uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
        uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
        uint8_t b = static_cast<uint8_t>(color.b * 255.0f);

        // last 255 is alpha
        uint32_t hex = (r << 24) | (g << 16) | (b << 8) | 0xFF;
        // uint32_t hex = (0xFF << 24) | (b << 16) | (g << 8) | r;

        return hex;
    }
}

class Texture {
    
    int width  = 0;
    int height = 0;

    uint32_t textureID = 0;             // (Address) - Points to pixelData in VRAM
    unsigned char* pixelData = nullptr; // (Raw Data) - RGBA

    // 0: SRGBA, 1: RGBA
    bool isRGBA;

public:
    
    Texture() = default;
    
    // Generate & Load a FlatColor Texture
    void load(const uint32_t color, const bool isRGBA);

    // Load an Image Texture
    void load(const std::string& path, const bool isRGBA); // CPU-Work

    // Compiles pixelData into VRAM
    void compile(); // GPU-Work

    void bind(const unsigned int textureUnit) const;

    // NOTE: USE ONLY IN DEVELOPMENT PHASE
    uint32_t getID() const { return textureID; }
};

class Line {

    static void renderHelper(
        const glm::mat4& model,
        const glm::vec3& color
    );
    
public:
    
    static uint VAO, VBO;
    static void init();

    static void render(
        const glm::vec3& point1,
        const glm::vec3& point2,
        const glm::vec3& color = glm::vec3(1.0f)
    );

    static void renderDirection(
        const glm::vec3& point,
        const glm::vec3& direction,
        const float length,
        const glm::vec3& color = glm::vec3(1.0f)
    );
};