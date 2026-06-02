#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <vector>

#include <lights.h>

// ------------------------------ Foward Declarations -------------------------------- //

#define MAX_ENTITIES 25
#define MAX_LIGHTS   10

class Shape;
class Shader;
class Texture2D;

using Entity = uint32_t;

// ------------------------------ Components ----------------------------------------- //

struct Transform {

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 model;
    glm::mat3 normalMatrix;

    void computeModel();
};

struct EntityMesh {

    Shape* shape;
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

class ECS {

    Entity nextEntity;
    
public:

    ECS();

    std::vector <Transform>  transforms;
    std::vector <EntityMesh> meshes;

    std::vector <MaterialPhong> materials;
    std::vector <MaterialPBR>   materialPBR; 

    std::vector <lights::PointLight> lightSources;
    std::vector <bool> hasPointLight;

    uint32_t total_lights;

    Entity createEntity() {
        return nextEntity++;
    }

    const Entity getTotal() const { return nextEntity; }
};

class RenderSystem {

    void bindCameraGlobals(const Shader* shader) const;
    void bindPointLightGlobals(const ECS& ecs) const;

    // For Lights
    // void draw(const EntityMesh& mesh, const Transform& transform, const MaterialLight& material) const;

    // For Phong
    void draw(const EntityMesh& mesh, const Transform& transform, const MaterialPhong& material) const;

public:

    static RenderSystem& instance() {

        static RenderSystem instance {};
        return instance;
    }

    // void update(const ECS& ecs) const;
    void render(const ECS& ecs) const;
};

class EntityShapes {

    ECS ecs;
    EntityShapes();

public:
    
    Entity cube;
    Entity square;
    
    static EntityShapes& instance() {

        static EntityShapes instance {};
        return instance;
    }

    const ECS& getECS() const { return ecs; }
};