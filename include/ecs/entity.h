#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <vector>
#include <iostream>

#include <ecs/component.h>

// ------------------------------ Foward Declarations -------------------------------- //

// WARNING: SHOULD BE SAME AS THAT DEFINED IN GLSL SHADER
#define MAX_LIGHTS 4

// ------------------------------ Foward Declarations -------------------------------- //

class Environment;

enum materialNames {

    MATERIAL_LIGHT,
    MATERIAL_PHONG,
    MATERIAL_PBR
};

class EntityShapes {
   
    EntityShapes();
    
public:

    MeshComponent cube;
    MeshComponent square;
    MeshComponent cubeNorm;
    
    InstanceComponent cubes;
    
    static EntityShapes& instance() {

        static EntityShapes instance {};
        return instance;
    }
};

class EntityManager {
    
    uint32_t total_objects;
    uint32_t total_lights;
    
    std::vector <Entity> destroyedEntities;
    
public:
    
    Environment* env;
    
    Entity nextEntity;
    Entity lastEntity;

    std::vector <Entity> visibleEntities;
    std::vector <Entity> emissiveEntities;

    EntityManager(): nextEntity(0) {};

    Entity createEntity() {

        if (nextEntity >= MAX_ENTITIES) {
            std::cerr << "ERROR::MAX ENTITIES REACHED!" << std::endl;
        }

        lastEntity = nextEntity;

        return nextEntity++;
    }

    const Entity& total_entities() const { return nextEntity; }
};

struct ECS {

    EntityManager    entityManager;
    ComponentManager componentManager;

    template <typename Component0>
    const std::vector<Entity> view() {

        std::vector <Entity> entities;

        for (Entity entity = 0; entity < entityManager.total_entities(); entity++) {
            if (componentManager.has<Component0>(entity)) entities.push_back(entity);
        }

        return entities;
    }

    template <typename Component0, typename Component1>
    const std::vector<Entity> view() {

        std::vector <Entity> entities;

        for (Entity entity = 0; entity < entityManager.total_entities(); entity++) {

            if (
                componentManager.has<Component0>(entity) &&
                componentManager.has<Component1>(entity)
            ) {
                entities.push_back(entity);
            }
        }

        return entities;
    }

    template <typename Component0, typename Component1, typename Component2>
    const std::vector<Entity> view() {

        std::vector <Entity> entities;

        for (Entity entity = 0; entity < entityManager.total_entities(); entity++) {

            if (
                componentManager.has<Component0>(entity) &&
                componentManager.has<Component1>(entity) &&
                componentManager.has<Component2>(entity)
            ) {
                entities.push_back(entity);
            }
        }

        return entities;
    }

    template <typename Component0>
    const std::vector<Entity> view() const {

        std::vector <Entity> entities;

        for (Entity entity = 0; entity < entityManager.total_entities(); entity++) {
            if (componentManager.has<Component0>(entity)) entities.push_back(entity);
        }

        return entities;
    }

    template <typename Component0, typename Component1>
    const std::vector<Entity> view() const {

        std::vector <Entity> entities;

        for (Entity entity = 0; entity < entityManager.total_entities(); entity++) {

            if (
                componentManager.has<Component0>(entity) &&
                componentManager.has<Component1>(entity)
            ) {
                entities.push_back(entity);
            }
        }

        return entities;
    }

    template <typename Component0, typename Component1, typename Component2>
    const std::vector<Entity> view() const {

        std::vector <Entity> entities;

        for (Entity entity = 0; entity < entityManager.total_entities(); entity++) {

            if (
                componentManager.has<Component0>(entity) &&
                componentManager.has<Component1>(entity) &&
                componentManager.has<Component2>(entity)
            ) {
                entities.push_back(entity);
            }
        }

        return entities;
    }

    void destroy(const Entity& entity);
};