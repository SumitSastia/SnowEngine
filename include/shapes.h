#pragma once

#include <s_math.h>

#include <vector>
#include <string>

class Texture2D;

// Hardcoded Model
class Shape {

protected:

    unsigned int VBO, VAO, EBO;
    unsigned int indicesCount;

    Texture2D* shapeDiffuseTexture;
    Texture2D* shapeNormalTexture;
    Texture2D* shapeSpecularTexture;

public:

    Shape(): 

        VBO(0),
        VAO(0),
        EBO(0),
        indicesCount(0),

        shapeDiffuseTexture(nullptr),
        shapeNormalTexture(nullptr),
        shapeSpecularTexture(nullptr) {
    }

    Shape copy() const {

        Shape _copy;
        
        _copy.VBO = this->VBO;
        _copy.VAO = this->VAO;
        _copy.EBO = this->EBO;

        _copy.indicesCount = this->indicesCount;

        _copy.shapeDiffuseTexture  = nullptr;
        _copy.shapeNormalTexture   = nullptr;
        _copy.shapeSpecularTexture = nullptr;

        return _copy;
    }

    // @note Temporary till Asset Manager is Initialized
    Texture2D* getAlbedo() const {
        return shapeDiffuseTexture;
    }

    // @note Temporary till Asset Manager is Initialized
    Texture2D* getNormalMap() const {
        return shapeNormalTexture;
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
    void destroy();
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

    unsigned int getCount() const { return instanceCounts; }
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