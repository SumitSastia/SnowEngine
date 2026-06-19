#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstdint>

#include <s_math.h>
#include <model.h>
#include <camera.h>

// ------------------------------ Foward Declarations -------------------------------- //

#define MAX_ENTITIES 5000
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

    glm::vec3 center;
    float radius;

    ShapeComponent():
        VAO(0), VBO(0), EBO(0),
        indicesCount(0), center(0.0f), radius(0.0f) {
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

    uint VAO, VBO, EBO;
    uint indicesCount;

    // local Radius
    float radius;

    // AABB
    glm::vec3 minCorner;
    glm::vec3 maxCorner;

    MeshComponent() = default;

    MeshComponent(uint VAO, uint VBO, uint EBO, uint indicesCount):
        VAO(VAO), VBO(VBO), EBO(EBO),
        indicesCount(indicesCount) {
    }

    /*
    vertices format:
    2f = aPos
    2f = aTexCords
    */
    void loadMesh2D(
        const std::vector<float>& vertices,
        const std::vector<uint>&  indices
    );

    /*
    vertices format:
    3f = aPos
    3f = aNormal
    2f = aTexCords
    */
    void loadMesh3D(
        const std::vector<float>& vertices,
        const std::vector<uint>&  indices
    );

    /*
    vertices format:
    3f = aPos
    3f = aNormal
    3f = aTangent
    2f = aTexCords
    */
    void loadMesh3DNormal(
        const std::vector<float>& vertices,
        const std::vector<uint>&  indices
    );

    void draw() const;
};

struct MeshLODComponent {

    MeshComponent high;
    MeshComponent moderate;
    MeshComponent low;

    const MeshComponent& getMesh(const float distance) const;
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
    Texture2D* specular;

    Texture2D* metallic;
    Texture2D* roughness;
    Texture2D* ao;

    MaterialComponent(): 
        shader(nullptr),
        albedo(nullptr),
        normal(nullptr),
        height(nullptr),
        specular(nullptr),
        metallic(nullptr),
        roughness(nullptr),
        ao(nullptr),
        gbufferShader(nullptr) {
    }
};

struct BoundingSphereComponent {
    glm::vec3 center;
    float radius;
};

struct BoundingAABBComponent {
    
    glm::vec3 center;

    glm::vec3 local_min;
    glm::vec3 local_max;

    glm::vec3 min;
    glm::vec3 max;

    // @note Define local_min & local_max with min & max
    // @warning AVOID USING THIS!!
    BoundingAABBComponent():
        min(FLT_MAX), max(-FLT_MAX) {
    }

    BoundingAABBComponent(
        const glm::vec3& center,
        const glm::vec3& min,
        const glm::vec3& max
    ):
        center(center),
        min(min), max(max),
        local_min(min), local_max(max) {
    }

    void recompute(const glm::mat4& model);
};

struct CameraComponent {

    Camera camera;
    // operator Camera&() { return camera; }
};

struct InstanceComponent {

    MeshComponent mesh;
    uint modelVBO, normalVBO;

    uint count;
    uint visibleCount;

    std::vector <TransformComponent>    transforms;
    std::vector <BoundingAABBComponent> AABBs;

    std::vector <const TransformComponent*> visibleInstances;

    InstanceComponent():
        count(0),
        transforms(MAX_INSTANCES) {
    }

    // Setups Mesh
    // @warning CALL ONLY ONCE!
    void bind(
        const std::vector<float>& vertices,
        const std::vector<uint>&  indices
    );

    // @note 'transforms' must be changed before calling this method.
    void updateModels();

    void draw() const;
};

struct ModelComponent {

    std::vector <MeshComponent>     meshes;
    std::vector <MaterialComponent> materials;

    float radius;

    ModelComponent() = default;

    ModelComponent(const Model3D* model, const MaterialComponent material) {
        init(model, material);
    }

    void init(const Model3D* model, const MaterialComponent material);
};

struct ModelLODComponent {

    ModelComponent high;
    ModelComponent moderate;
    ModelComponent low;

    const ModelComponent& getMesh(const float distance) const;
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
    ComponentPool <TransformComponent>      transforms;
    ComponentPool <MeshComponent>           meshes;
    ComponentPool <MaterialComponent>       materials;
    ComponentPool <InstanceComponent>       instances;
    ComponentPool <ModelComponent>          models;
    ComponentPool <PointLightComponent>     pointlights;
    ComponentPool <DirectionalLight>        directlights;
    ComponentPool <BoundingSphereComponent> boundingSpheres;
    ComponentPool <BoundingAABBComponent>   boundingAABBs;
    ComponentPool <MeshLODComponent>        meshLODs;
    ComponentPool <ModelLODComponent>       modelLODs;
    ComponentPool <CameraComponent>         cameras;
    
    // @note Currently only use for Model3D to Entity Conversion.
    void addShader(Shader* shader);

    // For Global uniform Bindings
    std::vector <Shader*> uniqueShaders;

    // For ShadowPass
    std::vector <PointShadowData>  pointShadowFrames;
    std::vector <DirectShadowData> directShadowFrames;

    template <typename Component>
    ComponentPool<Component>& getPool();

    template <typename Component>
    const ComponentPool<Component>& getPool() const;

    ComponentManager() = default;

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