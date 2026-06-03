#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstdint>

#include <s_math.h>

// ------------------------------ Foward Declarations -------------------------------- //

#define TOTAL_COMPONENTS 10
#define MAX_INSTANCES 100

class Shape;
class Shader;
class Texture2D;
class Model3D;

using Entity = uint32_t;

// ------------------------------ Components ----------------------------------------- //

struct ShapeComponent {

    uint VAO, VBO, EBO;
    uint indicesCount;

    ShapeComponent():
        VAO(0), VBO(0), EBO(0),
        indicesCount(0) {
    }

    ShapeComponent(uint VAO, uint VBO, uint EBO, uint indicesCount):
        VAO(VAO), VBO(VBO), EBO(EBO),
        indicesCount(indicesCount) {
    }

    void bindVertices2D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void bindVertices3D(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void bindVertices3D_Normal(
        const float* vertices, const size_t& size_v,
        const unsigned int* indices, const size_t& size_i
    );

    void draw() const;
};

struct TransformComponent {

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Matrix4   model;

    void computeModel();
};

struct MeshComponent {
    ShapeComponent shape;
};

// Light
struct MaterialLight {

    Shader* shader; // 8-Bytes
};

// Phong
struct MaterialPhong {

    Shader*    shader; // 8-Bytes
    Texture2D* albedo; // 8-Bytes
    Texture2D* normal; // 8-Bytes

    glm::vec3 color;   // 12-Bytes

    MaterialPhong():
        shader(nullptr),
        albedo(nullptr),
        normal(nullptr) {
    }
};

// PBR + IBL
struct MaterialPBR {

    Shader*    shader;    // 8-Bytes
    Texture2D* albedo;    // 8-Bytes
    Texture2D* normal;    // 8-Bytes

    Texture2D* metallic;  // 8-Bytes
    Texture2D* roughness; // 8-Bytes

    MaterialPBR():
        shader(nullptr),
        albedo(nullptr),
        normal(nullptr),
        metallic(nullptr),
        roughness(nullptr) {
    }
};

struct DirectionalLight {

    glm::vec3 direction; // 12-Bytes
    glm::vec3 color;     // 12-Bytes
};

struct PointLightComponent {

    glm::vec3 color;    // 12-Bytes

    float constant;     // 4-Bytes 
    float linear;       // 4-Bytes 
    float quadratic;    // 4-Bytes 
};

// Not suitable for ECS
struct SpotLight {

    glm::vec3 position;  // 12-Bytes
    glm::vec3 direction; // 12-Bytes
    glm::vec3 color;     // 12-Bytes

    float cutOffangle;   // 4-Bytes
    float outerCutOff;   // 4-Bytes
    
    float constant;      // 4-Bytes
    float linear;        // 4-Bytes
    float quadratic;     // 4-Bytes
    
    bool isVisible;      // 1-Byte
};

struct MaterialComponent {

    Shader*    shader;
    Texture2D* albedo;
    Texture2D* normal;

    MaterialComponent(): 
        shader(nullptr),
        albedo(nullptr),
        normal(nullptr) {
    }
};

struct InstanceComponent {

    uint VAO, VBO, EBO;
    uint modelVBO, normalVBO;

    uint indicesCount;
    uint count;

    std::vector <TransformComponent> transforms;

    InstanceComponent():
        count(0), indicesCount(0),
        transforms(MAX_INSTANCES) {
    }

    void bindVertices(
        const float*        vertices, const size_t& size_v,
        const unsigned int* indices,  const size_t& size_i
    );

    // To be implemented for Dynamic Entities
    void updateModels(
        const glm::mat4* models,  const size_t& size_m,
        const glm::mat3* normals, const size_t& size_n
    ) {}

    void draw() const;
};

struct ModelComponent {

    std::vector <MeshComponent>     meshes;
    std::vector <MaterialComponent> materials;

    void init(const Model3D* model, Shader* shader);
};

class ComponentManager {
    
public:
    
    void addShader(Shader* shader);
    
    // Renderable Objects
    std::vector <MeshComponent>      arr_mesh;
    std::vector <TransformComponent> arr_transform;
    std::vector <MaterialComponent>  arr_material;
    std::vector <InstanceComponent>  arr_instance;
    std::vector <ModelComponent>     arr_model;

    // Light Sources
    std::vector <PointLightComponent> arr_light;

    // For Objects to be lit by PointLights
    std::vector <Shader*> uniqueShaders;

    std::vector <bool> has_mesh;
    std::vector <bool> has_transform;
    std::vector <bool> has_material;
    std::vector <bool> has_instance;
    std::vector <bool> has_model;

    std::vector <bool> has_light;

    ComponentManager();

    template <typename Component>
    void addComponent(const Entity& entity, const Component& component);
};