#pragma once

#include <vector>
#include <cstdint>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "math/matrix.h"
#include "core/camera.h"
#include "core/config.h"

#include "utils/debug.h"

// ------------------------------ Foward Declarations -------------------------------- //

#define MAX_ENTITIES 5000
#define MAX_INSTANCES 100

#define TEXTURE_DEFAULT_ALBEDO 1U
#define TEXTURE_DEFAULT_NORMAL 2U

class PointShadowFrame;
class DirectShadowFrame;

using Entity = uint32_t;

// ------------------------------ Components ----------------------------------------- //

struct Vertex {

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCords;

    Vertex(
        const glm::vec3& position     = glm::vec3(0.0f),
        const glm::vec3& normal       = glm::vec3(0.0f),
        const glm::vec2& textureCords = glm::vec3(0.0f)
    ) :
        position(position),
        normal(normal),
        textureCords(textureCords) {
    }
};

struct Vertex_n {

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 bitangent;
    glm::vec2 textureCords;

    Vertex_n(
        const glm::vec3& position     = glm::vec3(0.0f),
        const glm::vec3& normal       = glm::vec3(0.0f),
        const glm::vec3& bitangent    = glm::vec3(0.0f),
        const glm::vec2& textureCords = glm::vec3(0.0f)
    ) :
        position(position),
        normal(normal),
        bitangent(bitangent),
        textureCords(textureCords) {
    }
};

struct TransformComponent {

    glm::vec3 position {0.0f};
    glm::vec3 rotation {0.0f};
    glm::vec3 scale    {1.0f};

    glm::vec3 local_position {0.0f};
    glm::vec3 local_rotation {0.0f};
    glm::vec3 local_scale    {1.0f};

    Matrix4 model;
    Matrix4 localModel;

    bool isVisible = true;

    void computeModel();
    void computePosition() { position = model.getMatrix()[3]; }

    void destroy() {}
};

struct MeshComponent {

    GLuint VAO, VBO, EBO;
    uint   indicesCount;

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
    void loadMesh3D(
        const std::vector<Vertex>& vertices,
        const std::vector<uint>&   indices
    );

    /*
    vertices format:
    3f = aPos
    3f = aNormal
    3f = aTangent
    2f = aTexCords
    */
    void loadMesh3D(
        const std::vector<Vertex_n>& vertices,
        const std::vector<uint>&     indices
    );

    void loadMesh3DNormal(
        const std::vector<float>& vertices,
        const std::vector<uint>&  indices
    );

    void draw() const;
    void destroy();
};

struct MeshLODComponent {

    MeshComponent high;
    MeshComponent moderate;
    MeshComponent low;

    const MeshComponent& getMesh(const float distance) const;

    void destroy();
};

struct DirectionalLight {

    glm::vec3 direction; // 12-Bytes
    glm::vec3 color;     // 12-Bytes

    void destroy() {}
};

struct PointLightComponent {

    glm::vec3 color;        // 12-Bytes

    float constant  = 0.0f; // 4-Bytes
    float linear    = 0.0f; // 4-Bytes
    float quadratic = 0.0f; // 4-Bytes

    void destroy() {}
};

struct SpotLightComponent {

    glm::vec3 position;  // 12-Bytes
    glm::vec3 direction; // 12-Bytes
    glm::vec3 color;     // 12-Bytes

    float cutOffangle;   // 4-Bytes
    float outerCutOff;   // 4-Bytes
    
    float constant;      // 4-Bytes
    float linear;        // 4-Bytes
    float quadratic;     // 4-Bytes
    
    bool isVisible;      // 1-Byte

    void destroy() {}
};

struct MaterialComponent {

    // Shader* shader;
    // Shader* gbufferShader;

    ShaderHandle shader        = 0;
    ShaderHandle gbufferShader = 0;
    
    TextureHandle albedo    = 0;
    TextureHandle normal    = 0;
    TextureHandle height    = 0;
    TextureHandle specular  = 0;
    TextureHandle metallic  = 0;
    TextureHandle roughness = 0;
    TextureHandle ao        = 0;

    // MaterialComponent(): 
    //     shader(nullptr),
    //     gbufferShader(nullptr) {
    // }

    MaterialComponent() = default;

    // SHOULD BE IMPLEMENTED WITH ASSET MANAGER
    void destroy() {}
};

struct AnimatedSprite {

    std::vector <TextureHandle> sprites;

    float transition_rate = 1.0f;
    float timer = 0.0f;

    uint32_t total_sprites = 0;
    TextureHandle activeSprite = 0;
};

struct BoundingSphereComponent {

    glm::vec3 center;
    float radius;

    void destroy() {}
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
    void destroy() {}
};

struct CameraComponent {

    Camera camera;
    // operator Camera&() { return camera; }

    void renderFrustum() const;
    void destroy() {}
};

struct InstanceComponent {

    MeshComponent mesh;
    GLuint modelVBO, normalVBO;

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
    void destroy();
};

class ModelComponent {
    
    void          processNode(aiNode* node, const aiScene* scene, const std::string& path, const bool normalMapped);
    MeshComponent processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path, const bool normalMapped);

    TextureHandle loadMaterialTexture(aiMaterial* material, aiTextureType type, const std::string& path);
    TextureHandle loadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& type_str, const std::string& path) { return 0; }
    
public:

    glm::vec3 minCorner;
    glm::vec3 maxCorner;
    
    std::vector <MeshComponent> meshes;
    std::vector <TextureHandle> textures;   // diffuse
    std::vector <TextureHandle> normalMaps; // normals

    float radius;

    ModelComponent() = default;
    ModelComponent(const std::string& path, const bool normalMapped);

    void destroy();
};

struct ModelLODComponent {

    ModelComponent high;
    ModelComponent moderate;
    ModelComponent low;

    const ModelComponent& getMesh(const float distance) const;
    void destroy();
};

struct ChildComponent {

    std::vector <Entity> children;
    
    // INCOMPLETE
    void destroy();
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

    void destroy();
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

    void destroy();
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

    void addComponent(const Entity entity) {
        hasData[entity] = true;
    }

    void addComponent(const Entity entity, const Component& component) {
        data[entity] = component;
        hasData[entity] = true;
    }

    void removeComponent(const Entity entity) {
        data[entity].destroy();
        hasData[entity] = false;
    }

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
    ComponentPool <ChildComponent>          childrens;
    ComponentPool <SpotLightComponent>      spotlights;
    ComponentPool <AnimatedSprite>          animSprites;
    
    // @note Currently only use for Model3D to Entity Conversion.
    void addShader(const ShaderHandle shader);

    // For Global uniform Bindings
    std::vector <ShaderHandle> uniqueShaders;

    // For ShadowPass
    std::vector <PointShadowData>  pointShadowFrames;
    std::vector <DirectShadowData> directShadowFrames;

    template <typename Component>
    ComponentPool<Component>& getPool();

    template <typename Component>
    const ComponentPool<Component>& getPool() const;

    ComponentManager() = default;

    template <typename Component>
    void addComponent(const Entity& entity) {
        getPool<Component>().addComponent(entity);
    }

    template <typename Component>
    void addComponent(const Entity& entity, const Component& component) {
        getPool<Component>().addComponent(entity, component);
        // std::cout << typeid(Component).name() << '\n';
    }

    template <typename Component>
    void removeComponent(const Entity& entity) {

        if (has<Component>(entity)) {
            getPool<Component>().removeComponent(entity);
        }
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

    template <typename Component>
    void copy(const Entity& src, const Entity& dst) {
        
        addComponent(
            dst,
            Component(get<Component>(src))
        );
    }
};

// --------------------------------------------------------------------------------------------- //

template <>
inline ComponentPool<TransformComponent>& ComponentManager::getPool() {
    return transforms;
}

template <>
inline ComponentPool<MeshComponent>& ComponentManager::getPool() {
    return meshes;
}

template <>
inline ComponentPool<MaterialComponent>& ComponentManager::getPool() {
    return materials;
}

template <>
inline ComponentPool<InstanceComponent>& ComponentManager::getPool() {
    return instances;
}

template <>
inline ComponentPool<ModelComponent>& ComponentManager::getPool() {
    return models;
}

template <>
inline ComponentPool<PointLightComponent>& ComponentManager::getPool() {
    return pointlights;
}

template <>
inline ComponentPool<DirectionalLight>& ComponentManager::getPool() {
    return directlights;
}

template <>
inline ComponentPool<BoundingSphereComponent>& ComponentManager::getPool() {
    return boundingSpheres;
}

template <>
inline ComponentPool<BoundingAABBComponent>& ComponentManager::getPool() {
    return boundingAABBs;
}

template <>
inline ComponentPool<MeshLODComponent>& ComponentManager::getPool() {
    return meshLODs;
}

template <>
inline ComponentPool<ModelLODComponent>& ComponentManager::getPool() {
    return modelLODs;
}

template <>
inline ComponentPool<CameraComponent>& ComponentManager::getPool() {
    return cameras;
}

template <>
inline ComponentPool<ChildComponent>& ComponentManager::getPool() {
    return childrens;
}

template <>
inline ComponentPool<SpotLightComponent>& ComponentManager::getPool() {
    return spotlights;
}

template <>
inline ComponentPool<AnimatedSprite>& ComponentManager::getPool() {
    return animSprites;
}

template <>
inline const ComponentPool<TransformComponent>& ComponentManager::getPool() const {
    return transforms;
}

template <>
inline const ComponentPool<MeshComponent>& ComponentManager::getPool() const {
    return meshes;
}

template <>
inline const ComponentPool<MaterialComponent>& ComponentManager::getPool() const {
    return materials;
}

template <>
inline const ComponentPool<InstanceComponent>& ComponentManager::getPool() const {
    return instances;
}

template <>
inline const ComponentPool<ModelComponent>& ComponentManager::getPool() const {
    return models;
}

template <>
inline const ComponentPool<PointLightComponent>& ComponentManager::getPool() const {
    return pointlights;
}

template <>
inline const ComponentPool<DirectionalLight>& ComponentManager::getPool() const {
    return directlights;
}

template <>
inline const ComponentPool<BoundingSphereComponent>& ComponentManager::getPool() const {
    return boundingSpheres;
}

template <>
inline const ComponentPool<BoundingAABBComponent>& ComponentManager::getPool() const {
    return boundingAABBs;
}

template <>
inline const ComponentPool<MeshLODComponent>& ComponentManager::getPool() const {
    return meshLODs;
}

template <>
inline const ComponentPool<ModelLODComponent>& ComponentManager::getPool() const {
    return modelLODs;
}

template <>
inline const ComponentPool<CameraComponent>& ComponentManager::getPool() const {
    return cameras;
}

template <>
inline const ComponentPool<ChildComponent>& ComponentManager::getPool() const {
    return childrens;
}

template <>
inline const ComponentPool<SpotLightComponent>& ComponentManager::getPool() const {
    return spotlights;
}

template <>
inline const ComponentPool<AnimatedSprite>& ComponentManager::getPool() const {
    return animSprites;
}

// template <>
// inline void ComponentManager::addComponent<MaterialComponent>(const Entity& entity, const MaterialComponent& component) {

//     getPool<MaterialComponent>().addComponent(entity, component);
//     addShader(component.shader);
// }

// template <>
// inline void ComponentManager::addComponent<PointLightComponent>(const Entity& entity, const PointLightComponent& component) {

//     getPool<PointLightComponent>().addComponent(entity, component);

//     pointShadowFrames.push_back(
//         PointShadowData(entity, new PointShadowFrame())
//     );
// }