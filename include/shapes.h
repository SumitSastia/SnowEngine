#pragma once

#include <entity.h>
#include <s_math.h>

#include <vector>
#include <string>

class Texture2D;

// Hardcoded Model
class Shape : public Entity {

protected:

    unsigned int VBO, VAO, EBO;
    unsigned int indicesCount;

    Texture2D* shapeDiffuseTexture;
    Texture2D* shapeNormalTexture;
    Texture2D* shapeSpecularTexture;

public:

    Shape(): 
        shapeDiffuseTexture(nullptr),
        shapeNormalTexture(nullptr),
        shapeSpecularTexture(nullptr) {
    }

    void bindVertices2D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void bindVertices3D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void bindVertices3D_Mapped(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    const unsigned int get_VAO() const { return VAO; }
    const unsigned int get_indices() const { return indicesCount; }

    void loadDiffuseTex (const char* path);
    void loadNormalTex  (const char* path);
    void loadSpecularTex(const char* path);

    void bindDiffuseTex (const unsigned int textureUnit) const;
    void bindNormalTex  (const unsigned int textureUnit) const;
    void bindSpecularTex(const unsigned int textureUnit) const;
    
    void draw() const;
};

class ShapeInstanced : public Shape {

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
    Shape advancedCube;

    DefaultShapes();
    static DefaultShapes& instance();
};