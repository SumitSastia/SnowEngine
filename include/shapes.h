#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <entity.h>

#include <vector>
#include <string>

class Texture2D;

// Hardcoded Model
class Shape : public Entity {

protected:

    unsigned int VBO, VAO, EBO;
    unsigned int indicesCount;

    Texture2D* shapeDiffuseTexture;

public:

    Shape();

    void bindVertices2D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void bindVertices3D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    const unsigned int get_VAO() const { return VAO; }
    const unsigned int get_indices() const { return indicesCount; }

    void bindTexture(const unsigned int textureUnit) const;
    void loadTexture(const char* diffusePath);
    void draw() const;
};

// Model with Specular Texture
class SpecShape : public Shape {

protected:
    Texture2D* shapeSpecularTexture;

public:

    SpecShape();
    void loadTexture(const char* diffusePath, const char* specularPath);
};

class ShapeInstanced : public SpecShape {

    unsigned int instanceModelVBO, instanceNormalVBO;
    unsigned int instanceCounts;

public:

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
struct DefaultShapes {

    Shape square;
    Shape cube;
    ShapeInstanced cubeInstanced;

    DefaultShapes();
    static DefaultShapes& instance();
};

class CubeMap {
    
    unsigned int VBO, VAO, textureID;

public:

    CubeMap(const std::vector <std::string>& textureFaces);

    const unsigned int& get_VAO() const { return VAO; }
    const unsigned int& get_ID() const { return textureID; }
};