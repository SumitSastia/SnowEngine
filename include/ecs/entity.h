#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <vector>

#include <ecs/component.h>

// ------------------------------ Foward Declarations -------------------------------- //

#define MAX_ENTITIES 25
#define MAX_LIGHTS   10

// ------------------------------ Foward Declarations -------------------------------- //

// using Entity = uint32_t;

enum materialNames {

    MATERIAL_LIGHT,
    MATERIAL_PHONG,
    MATERIAL_PBR
};

class EntityShapes {
   
    EntityShapes();
    
public:

    ShapeComponent cube;
    ShapeComponent square;
    ShapeComponent cubeNormalMapped;
    
    InstanceComponent cubes;
    
    static EntityShapes& instance() {

        static EntityShapes instance {};
        return instance;
    }
};

class EntityManager {

    Entity nextEntity;

    uint32_t total_objects;
    uint32_t total_lights;

public:

    std::vector <Entity> visibleEntities;
    std::vector <Entity> emissiveEntities;

    EntityManager();

    Entity createEntity() { return nextEntity++; }
    const Entity& total_entities() const { return nextEntity; }
};