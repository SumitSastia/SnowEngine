#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <ecs/component.h>

struct MeshTexture {

    unsigned int id;
    std::string type;
    std::string checkPath;

    void loadTexture(const char* path, const std::string& directory);
};

class Mesh {

public:
    
    unsigned int VBO, VAO, EBO;
    void setupMesh();

    std::vector <Vertex>       vertices;
    std::vector <unsigned int> indices;
    std::vector <MeshTexture>  textures;

    Mesh(
        const std::vector <Vertex>& vertices,
        const std::vector <unsigned int>& indices,
        const std::vector <MeshTexture>& textures
    ) :
        vertices(vertices),
        indices(indices),
        textures(textures),
        VBO(0), VAO(0), EBO(0) {

        setupMesh();
    }

    // Disable Copying
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Move
    Mesh(Mesh&& other) noexcept {
        *this = std::move(other);
    }

    Mesh& operator=(Mesh&& other) noexcept {

        VBO = other.VBO;
        VAO = other.VAO;
        EBO = other.EBO;

        vertices = other.vertices;
        indices = other.indices;
        textures = other.textures;

        other.VBO = other.VAO = other.EBO = 0;
        return *this;
    }

    void bindTextures(const unsigned int textureUnit) const;
    void draw() const;
    void destroy();
    
    // clears vertices, indices, textures
    // @warning Must only be called my Model3D.
    // @note Vertex Array Buffer still exists.
    void clean();
};

class Model3D {

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;

    uint16_t total_mesh;
    std::string directory;

    float radius;
    
    void loadModel(const std::string& path);
    
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    
    std::vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& type_str) const;
    
public:

    glm::vec3 minCorner;
    glm::vec3 maxCorner;
    
    std::vector <Mesh> meshes;
    std::vector <MeshTexture> loadedTextures;

    Model3D(const char* path):
        directory(""), 
        total_mesh(0),
        radius(0.0f),
        minCorner(FLT_MAX),
        maxCorner(-FLT_MAX)
    {
        loadModel(path);
    }

    // Binds Textures of the Model into the Memory GL_TEXTURE0 and so on
    void bindTextures(const unsigned int firstTextureUnit) const;

    void draw() const;
    void destroy();

    // Cleans meshes, loadedTextures after converting them to ECS Component.
    float clean();
};

//////////////////////////////////////////////////////////////////////////////////

// class ModelLoader {

//     enum MODEL_MATERIAL_TYPE {
//         FLATCOLOR_MATERIAL,
//         PBR_MATERIAL,
//         PHONG_MATERIAL
//     };

//     ModelComponent    model;
//     MaterialComponent material;

// public:

//     ModelLoader(const std::string& path): model(path) {}
// };