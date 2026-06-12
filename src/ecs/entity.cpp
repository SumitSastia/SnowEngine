#include <ecs/rendersystem.h>

#include <shader.h>
#include <shapes.h>
#include <lights.h>

// ECS::ECS():
//     hasPointLight(MAX_ENTITIES, false)
// {
//     transforms.reserve(MAX_ENTITIES);
//     meshes.reserve(MAX_ENTITIES);
//     materialType.reserve(MAX_ENTITIES);

//     materials.reserve(MAX_ENTITIES);
//     materialLight.reserve(MAX_ENTITIES);
//     materialPBR.reserve(MAX_ENTITIES);

//     lightSources.reserve(MAX_ENTITIES);
// }

EntityShapes::EntityShapes() {

    const float verticesCube[] = {

        // Front
        -0.5, 0.5, 0.5,  0.0,0.0, 1.0, 0.0,0.0,
         0.5, 0.5, 0.5,  0.0,0.0, 1.0, 1.0,0.0,
        -0.5,-0.5, 0.5,  0.0,0.0, 1.0, 0.0,1.0,
         0.5,-0.5, 0.5,  0.0,0.0, 1.0, 1.0,1.0,
         
        // Back
         0.5, 0.5,-0.5,  0.0,0.0,-1.0, 0.0,0.0,
        -0.5, 0.5,-0.5,  0.0,0.0,-1.0, 1.0,0.0,
         0.5,-0.5,-0.5,  0.0,0.0,-1.0, 0.0,1.0,
        -0.5,-0.5,-0.5,  0.0,0.0,-1.0, 1.0,1.0,

        // Left
        -0.5, 0.5,-0.5,  -1.0,0.0,0.0, 0.0,0.0,
        -0.5, 0.5, 0.5,  -1.0,0.0,0.0, 1.0,0.0,
        -0.5,-0.5,-0.5,  -1.0,0.0,0.0, 0.0,1.0,
        -0.5,-0.5, 0.5,  -1.0,0.0,0.0, 1.0,1.0,

        // Right
         0.5, 0.5, 0.5,   1.0,0.0,0.0, 0.0,0.0,
         0.5, 0.5,-0.5,   1.0,0.0,0.0, 1.0,0.0,
         0.5,-0.5, 0.5,   1.0,0.0,0.0, 0.0,1.0,
         0.5,-0.5,-0.5,   1.0,0.0,0.0, 1.0,1.0,

        // Top
        -0.5, 0.5,-0.5,  0.0, 1.0,0.0, 0.0,0.0,
         0.5, 0.5,-0.5,  0.0, 1.0,0.0, 1.0,0.0,
        -0.5, 0.5, 0.5,  0.0, 1.0,0.0, 0.0,1.0,
         0.5, 0.5, 0.5,  0.0, 1.0,0.0, 1.0,1.0,

        // Bottom
         0.5,-0.5,-0.5,  0.0,-1.0,0.0, 0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,-1.0,0.0, 1.0,0.0,
         0.5,-0.5, 0.5,  0.0,-1.0,0.0, 0.0,1.0,
        -0.5,-0.5, 0.5,  0.0,-1.0,0.0, 1.0,1.0
    };

    const unsigned int indicesCube[] = {

        // Front
        0,1,2,
        1,3,2,

        // Back
        4,5,6,
        5,7,6,

        // Left
        8,9,10,
        9,11,10,

        // Right
        12,13,14,
        13,15,14,

        // Top
        16,17,18,
        17,19,18,

        // Bottom
        20,21,22,
        21,23,22
    };
   
    const float verticesSquare[] = {
        
        -0.5f, 0.5f, 0.0f,0.0f,
         0.5f, 0.5f, 1.0f,0.0f,
        -0.5f,-0.5f, 0.0f,1.0f,
         0.5f,-0.5f, 1.0f,1.0f
    };

    const unsigned int indicesSquare[] = {

        0,1,2,
        1,3,2
    };

    const float verticesCubeNormalMapped[] = {

        // Front         // Normals     // UV     // Tangents
        -0.5, 0.5, 0.5,  0.0,0.0, 1.0,  0.0,0.0,  1.0,0.0,0.0,
         0.5, 0.5, 0.5,  0.0,0.0, 1.0,  1.0,0.0,  1.0,0.0,0.0,
        -0.5,-0.5, 0.5,  0.0,0.0, 1.0,  0.0,1.0,  1.0,0.0,0.0,
         0.5,-0.5, 0.5,  0.0,0.0, 1.0,  1.0,1.0,  1.0,0.0,0.0,
         
        // Back
         0.5, 0.5,-0.5,  0.0,0.0,-1.0,  0.0,0.0, -1.0,0.0,0.0,
        -0.5, 0.5,-0.5,  0.0,0.0,-1.0,  1.0,0.0, -1.0,0.0,0.0,
         0.5,-0.5,-0.5,  0.0,0.0,-1.0,  0.0,1.0, -1.0,0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,0.0,-1.0,  1.0,1.0, -1.0,0.0,0.0,

        // Left
        -0.5, 0.5,-0.5,  -1.0,0.0,0.0,  0.0,0.0,  0.0,0.0,1.0,
        -0.5, 0.5, 0.5,  -1.0,0.0,0.0,  1.0,0.0,  0.0,0.0,1.0,
        -0.5,-0.5,-0.5,  -1.0,0.0,0.0,  0.0,1.0,  0.0,0.0,1.0,
        -0.5,-0.5, 0.5,  -1.0,0.0,0.0,  1.0,1.0,  0.0,0.0,1.0,

        // Right
         0.5, 0.5, 0.5,  1.0,0.0,0.0,   0.0,0.0, 0.0,0.0,-1.0,
         0.5, 0.5,-0.5,  1.0,0.0,0.0,   1.0,0.0, 0.0,0.0,-1.0,
         0.5,-0.5, 0.5,  1.0,0.0,0.0,   0.0,1.0, 0.0,0.0,-1.0,
         0.5,-0.5,-0.5,  1.0,0.0,0.0,   1.0,1.0, 0.0,0.0,-1.0,

        // Top
        -0.5, 0.5,-0.5,  0.0, 1.0,0.0,  0.0,0.0,  1.0,0.0,0.0,
         0.5, 0.5,-0.5,  0.0, 1.0,0.0,  1.0,0.0,  1.0,0.0,0.0,
        -0.5, 0.5, 0.5,  0.0, 1.0,0.0,  0.0,1.0,  1.0,0.0,0.0,
         0.5, 0.5, 0.5,  0.0, 1.0,0.0,  1.0,1.0,  1.0,0.0,0.0,

        // Bottom
         0.5,-0.5,-0.5,  0.0,-1.0,0.0,  0.0,0.0,  1.0,0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,-1.0,0.0,  1.0,0.0,  1.0,0.0,0.0,
         0.5,-0.5, 0.5,  0.0,-1.0,0.0,  0.0,1.0,  1.0,0.0,0.0,
        -0.5,-0.5, 0.5,  0.0,-1.0,0.0,  1.0,1.0,  1.0,0.0,0.0
    };

    cube.bindVertices3D(verticesCube, sizeof(verticesCube), indicesCube, sizeof(indicesCube));
    square.bindVertices2D(verticesSquare, sizeof(verticesSquare), indicesSquare, sizeof(indicesSquare));

    cubeNormalMapped.bindVertices3D_Normal(verticesCubeNormalMapped, sizeof(verticesCubeNormalMapped), indicesCube, sizeof(indicesCube));

    // Instance Cubes
    glm::vec3 cubePositions[] = {

        glm::vec3(5.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    cubes.count = 10;

    for (uint32_t i = 0; i < 10; i++) {

        cubes.transforms[i].position = cubePositions[i];
        cubes.transforms[i].rotation = (i * 12.0f * glm::vec3(1.0f, 2.0f, 3.0f));
        cubes.transforms[i].scale    = glm::vec3(0.8);

        cubes.transforms[i].computeModel();
    }

    cubes.bindVertices(
        verticesCube, sizeof(verticesCube), 
        indicesCube,  sizeof(indicesCube)
    );
}

EntityManager::EntityManager(): nextEntity(0) {

}

// template <typename Component0>
// const std::vector<Entity>& ECS::view() const {

//     std::vector <Entity> entities;

//     for (const Entity entity : entityManager.total_entities) {
//         if (componentManager.has<Component0>(entity)) entities.push_back(entity);
//     }

//     return entities;
// }

// template <typename Component0, typename Component1>
// const std::vector<Entity>& ECS::view() const {

//     std::vector <Entity> entities;

//     for (const Entity entity : entityManager.total_entities) {

//         if (
//             componentManager.has<Component0>(entity) &&
//             componentManager.has<Component1>(entity)
//         ) {
//             entities.push_back(entity);
//         }
//     }

//     return entities;
// }

// template <typename Component0, typename Component1, typename Component2>
// const std::vector<Entity>& ECS::view() const {

//     std::vector <Entity> entities;

//     for (const Entity entity : entityManager.total_entities) {

//         if (
//             componentManager.has<Component0>(entity) &&
//             componentManager.has<Component1>(entity) &&
//             componentManager.has<Component2>(entity)
//         ) {
//             entities.push_back(entity);
//         }
//     }

//     return entities;
// }