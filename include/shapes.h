#pragma once

#include <shader.h>
#include <vector>

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


// Hardcoded Model
struct shape {

    unsigned int VBO, VAO, EBO;
    unsigned int indicesCount;

    texture2D shapeDiffuseTexture;

    void bindVertices2D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void bindVertices3D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void loadTexture(const char* diffusePath);
    void draw() const;
};

// Model with Specular Texture
struct specShape : public shape {

    texture2D shapeSpecularTexture;
    void loadTexture(const char* diffusePath, const char* specularPath);
};

struct shapeInstanced : specShape {

    unsigned int instanceModelVBO, instanceNormalVBO;
    unsigned int instanceCounts;

    void bindVertices(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i,
        const glm::mat4* models, const size_t& size_m,
        const glm::mat3* normals, const size_t& size_n
    );

    void draw() const;
};

/*
Hardcoded Shapes
(Already allocated in the Memory)
*/
struct defaultShapes {

    shape square;
    specShape cube;
    shapeInstanced cubeInstanced;

    defaultShapes();
    static defaultShapes& instance();
};

class cubeMap {
    
    unsigned int VBO, VAO, textureID;

public:

    cubeMap(const std::vector <std::string>& textureFaces);

    const unsigned int& get_VAO() const { return VAO; }
    const unsigned int& get_ID() const { return textureID; }
};