#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstdint>

#include <s_math.h>

// ------------------------------ Foward Declarations -------------------------------- //

#define MAX_ENTITIES 25
#define TOTAL_COMPONENTS 10
#define MAX_INSTANCES 100

class Shape;
class Shader;
class Texture2D;
class Model3D;
class PointShadowFrame;
class DirectShadowFrame;

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
    void computePosition() { position = model.getMatrix()[3]; }
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

    Shader* shader;
    Shader* gbufferShader;
    
    Texture2D* albedo;
    Texture2D* normal;
    Texture2D* height;

    Texture2D* metallic;
    Texture2D* roughness;
    Texture2D* ao;

    MaterialComponent(): 
        shader(nullptr),
        albedo(nullptr),
        normal(nullptr),
        height(nullptr),
        metallic(nullptr),
        roughness(nullptr),
        ao(nullptr),
        gbufferShader(nullptr) {
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

    void init(const Model3D* model, Shader* shader, Shader* gbufferShader);
};

struct PointShadowData {

    Entity entity;
    PointShadowFrame* frame;

    PointShadowData():
        entity(0), frame(nullptr) {
    }

    PointShadowData(Entity entity, PointShadowFrame* frame):
        entity(entity), frame(frame) {
    }
};

struct DirectShadowData {

    Entity  entity;
    Matrix4 lightSpaceMatrix;
    DirectShadowFrame* frame;

    DirectShadowData():
        entity(0), lightSpaceMatrix(Matrix4(1.0f)), frame(nullptr) {
    }

    DirectShadowData(Entity entity, Matrix4 matrix, DirectShadowFrame* frame):
        entity(entity), lightSpaceMatrix(matrix), frame(frame) {
    }
};

template <typename Component>
class ComponentPool {
    
    std::vector <Component> data;
    std::vector <bool> hasData;

public:

    ComponentPool():
        data(MAX_ENTITIES),
        hasData(MAX_ENTITIES, false) {
    }

    void addComponent(const Entity entity, const Component& component) {
        data[entity] = component;
        hasData[entity] = true;
    }

    void removeComponent(const Entity entity) {}

    const bool hasComponent(const Entity entity) const {
        return hasData[entity];
    }

    const Component& getComponent(const Entity entity) const {
        return data[entity];
    }

    Component& getComponent(const Entity entity) {
        return data[entity];
    }
};

class ComponentManager {

public:

    // Component Pools
    ComponentPool <TransformComponent>  transforms;
    ComponentPool <MeshComponent>       meshes;
    ComponentPool <MaterialComponent>   materials;
    ComponentPool <InstanceComponent>   instances;
    ComponentPool <ModelComponent>      models;
    ComponentPool <PointLightComponent> pointlights;
    ComponentPool <DirectionalLight>    directlights;
    
    // @note Currently only use for Model3D to Entity Conversion.
    void addShader(Shader* shader);
    
    // For Renderable Objects
    std::vector <MeshComponent>      arr_mesh;
    std::vector <TransformComponent> arr_transform;
    std::vector <MaterialComponent>  arr_material;
    std::vector <InstanceComponent>  arr_instance;
    std::vector <ModelComponent>     arr_model;

    // For Light Sources
    std::vector <PointLightComponent> arr_light;
    std::vector <DirectionalLight>    arr_directionalLight;

    // For Objects to be lit by PointLights
    std::vector <Shader*> uniqueShaders;

    // For ShadowPass
    std::vector <PointShadowData>  pointShadowFrames;
    std::vector <DirectShadowData> directShadowFrames;

    // Verifiers
    std::vector <bool> has_mesh;
    std::vector <bool> has_transform;
    std::vector <bool> has_material;
    std::vector <bool> has_instance;
    std::vector <bool> has_model;
    std::vector <bool> has_light;
    std::vector <bool> has_directionalLight;

    template <typename Component>
    ComponentPool<Component>& getPool();

    template <typename Component>
    const ComponentPool<Component>& getPool() const;

    ComponentManager();

    // template <typename Component>
    // void addComponent(const Entity& entity, const Component& component);

    template <typename Component>
    void addComponent(const Entity& entity, const Component& component) {
        getPool<Component>().addComponent(entity, component);
    }

    template <typename Component>
    bool has(const Entity& entity) const {
        return getPool<Component>().hasComponent(entity);
    }

    template <typename Component>
    const Component& get(const Entity& entity) const {
        return getPool<Component>().getComponent(entity);
    }

    template <typename Component>
    Component& get(const Entity& entity) {
        return getPool<Component>().getComponent(entity);
    }
};