#pragma once

#include <shader.h>

// Colored Models
struct coloredCube {

    unsigned int VBO, VAO, EBO;
    unsigned int colorShader;

    glm::vec3 color;

    coloredCube(const glm::vec3& color);
    ~coloredCube();

    void update(const float& delta_time);
    void render() const;
};

struct shape2D {

    unsigned int VBO, VAO, EBO;
    unsigned int indicesCount;

    texture shape2DTexture;
    glm::vec3 normal;

    shape2D() : normal(glm::vec3(0.0f, 0.0f, 1.0f)) {}
    ~shape2D();

    void bindVertices(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void loadTexture(const char* texturePath);
    void draw(const unsigned int& shader, const glm::mat4& model) const;
    void draw_gbuffer(const unsigned int& shader, const glm::mat4& model) const;
    void drawShadow() const;
};

// Hardcoded Models
struct shape {

    unsigned int VBO, VAO, EBO;
    unsigned int indicesCount;

    texture shapeDiffuseTexture;

    void bindVertices(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void loadTexture(const char* diffusePath);
    void draw(const unsigned int& shader, const glm::mat4& model) const;
    void draw_gbuffer(const unsigned int& shader, const glm::mat4& model) const;
    void drawShadow() const;
};

struct specShape : public shape {

    texture shapeSpecularTexture;

    ~specShape();

    void loadTexture(const char* diffusePath, const char* specularPath);
    void draw(const unsigned int& shader, const glm::mat4& model) const;
    void draw_gbuffer(const unsigned int& shader, const glm::mat4& model) const;
};

struct shapeInstanced : specShape {

    void bindVertices(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void draw(const unsigned int& shader, const unsigned int& instanceCounts) const;
    void draw_gbuffer(const unsigned int& shader, const unsigned int& instanceCounts) const;

    void drawShadow(const unsigned int& instanceCounts) const;
};

struct shapes {

    shape2D square;
    specShape cube;
    shapeInstanced cubeInstanced;

    shapes();
    static shapes& instance();
};

class cubeMap {
    
    unsigned int VBO, VAO, textureID;

public:

    cubeMap(const std::vector <std::string>& textureFaces);
    ~cubeMap();

    const unsigned int& get_VAO() const { return VAO; }
    const unsigned int& get_ID() const { return textureID; }
};