#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <vector>

// ------------------------------ Foward Declarations -------------------------------- //

#define MAX_ENTITIES 25

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
struct Material {

    Shader* shader;

    Texture2D* albedo;
    Texture2D* normal;

    Material():
        shader(nullptr),
        albedo(nullptr),
        normal(nullptr) {
    }
};

// PBR + IBL
struct MaterialPBR {

    Shader* shader;

    Texture2D* albedo;
    Texture2D* normal;

    Texture2D* metallic;
    Texture2D* roughness;

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

    ECS():
        nextEntity(0),
        transforms(MAX_ENTITIES),
        meshes(MAX_ENTITIES),
        materials(MAX_ENTITIES) {
    }

    std::vector <Transform>  transforms;
    std::vector <EntityMesh> meshes;
    std::vector <Material>   materials;

    Entity createEntity() {

        transforms.push_back(Transform{});
        meshes.push_back(EntityMesh{});
        materials.push_back(Material{});

        return nextEntity++;
    }

    const Entity getTotal() const { return nextEntity; }
};

class RenderSystem {

    void bindCameraGlobals(const Shader* shader) const;
    void draw(const EntityMesh& mesh, const Transform& transform, const Material& material) const;

public:

    static RenderSystem& instance() {

        static RenderSystem instance {};
        return instance;
    }

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