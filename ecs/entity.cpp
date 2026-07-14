#include "core/lights.h"
#include "core/shader/shader.h"
#include "ecs/systems/rendersystem.h"

EntityShapes::EntityShapes() {

    const std::vector <float> vertices = {
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

    const std::vector <uint> indices = {
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

    const std::vector <float> verticesSqr = {
        -0.5f, 0.5f, 0.0f,0.0f,
         0.5f, 0.5f, 1.0f,0.0f,
        -0.5f,-0.5f, 0.0f,1.0f,
         0.5f,-0.5f, 1.0f,1.0f
    };

    const std::vector <uint> indicesSqr = {
        0,1,2,
        1,3,2
    };

    const std::vector <float> verticesNorm = {

        // Front         // Normals     // Tangents   // UV
        -0.5, 0.5, 0.5,  0.0,0.0, 1.0,  1.0,0.0,0.0,  0.0,0.0,
         0.5, 0.5, 0.5,  0.0,0.0, 1.0,  1.0,0.0,0.0,  1.0,0.0,
        -0.5,-0.5, 0.5,  0.0,0.0, 1.0,  1.0,0.0,0.0,  0.0,1.0,
         0.5,-0.5, 0.5,  0.0,0.0, 1.0,  1.0,0.0,0.0,  1.0,1.0,
         
        // Back
         0.5, 0.5,-0.5,  0.0,0.0,-1.0, -1.0,0.0,0.0,  0.0,0.0,
        -0.5, 0.5,-0.5,  0.0,0.0,-1.0, -1.0,0.0,0.0,  1.0,0.0,
         0.5,-0.5,-0.5,  0.0,0.0,-1.0, -1.0,0.0,0.0,  0.0,1.0,
        -0.5,-0.5,-0.5,  0.0,0.0,-1.0, -1.0,0.0,0.0,  1.0,1.0,

        // Left
        -0.5, 0.5,-0.5,  -1.0,0.0,0.0,  0.0,0.0,1.0,  0.0,0.0,
        -0.5, 0.5, 0.5,  -1.0,0.0,0.0,  0.0,0.0,1.0,  1.0,0.0,
        -0.5,-0.5,-0.5,  -1.0,0.0,0.0,  0.0,0.0,1.0,  0.0,1.0,
        -0.5,-0.5, 0.5,  -1.0,0.0,0.0,  0.0,0.0,1.0,  1.0,1.0,

        // Right
         0.5, 0.5, 0.5,  1.0,0.0,0.0,  0.0,0.0,-1.0,  0.0,0.0,
         0.5, 0.5,-0.5,  1.0,0.0,0.0,  0.0,0.0,-1.0,  1.0,0.0,
         0.5,-0.5, 0.5,  1.0,0.0,0.0,  0.0,0.0,-1.0,  0.0,1.0,
         0.5,-0.5,-0.5,  1.0,0.0,0.0,  0.0,0.0,-1.0,  1.0,1.0,

        // Top
        -0.5, 0.5,-0.5,  0.0, 1.0,0.0,  1.0,0.0,0.0,  0.0,0.0,
         0.5, 0.5,-0.5,  0.0, 1.0,0.0,  1.0,0.0,0.0,  1.0,0.0,
        -0.5, 0.5, 0.5,  0.0, 1.0,0.0,  1.0,0.0,0.0,  0.0,1.0,
         0.5, 0.5, 0.5,  0.0, 1.0,0.0,  1.0,0.0,0.0,  1.0,1.0,

        // Bottom
         0.5,-0.5,-0.5,  0.0,-1.0,0.0,  1.0,0.0,0.0,  0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,-1.0,0.0,  1.0,0.0,0.0,  1.0,0.0,
         0.5,-0.5, 0.5,  0.0,-1.0,0.0,  1.0,0.0,0.0,  0.0,1.0,
        -0.5,-0.5, 0.5,  0.0,-1.0,0.0,  1.0,0.0,0.0,  1.0,1.0 
    };

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
   
    // const float verticesSquare[] = {
        
    //     -0.5f, 0.5f, 0.0f,0.0f,
    //      0.5f, 0.5f, 1.0f,0.0f,
    //     -0.5f,-0.5f, 0.0f,1.0f,
    //      0.5f,-0.5f, 1.0f,1.0f
    // };

    // const unsigned int indicesSquare[] = {

    //     0,1,2,
    //     1,3,2
    // };

    // const float verticesCubeNormalMapped[] = {

    //     // Front         // Normals     // UV     // Tangents
    //     -0.5, 0.5, 0.5,  0.0,0.0, 1.0,  0.0,0.0,  1.0,0.0,0.0,
    //      0.5, 0.5, 0.5,  0.0,0.0, 1.0,  1.0,0.0,  1.0,0.0,0.0,
    //     -0.5,-0.5, 0.5,  0.0,0.0, 1.0,  0.0,1.0,  1.0,0.0,0.0,
    //      0.5,-0.5, 0.5,  0.0,0.0, 1.0,  1.0,1.0,  1.0,0.0,0.0,
         
    //     // Back
    //      0.5, 0.5,-0.5,  0.0,0.0,-1.0,  0.0,0.0, -1.0,0.0,0.0,
    //     -0.5, 0.5,-0.5,  0.0,0.0,-1.0,  1.0,0.0, -1.0,0.0,0.0,
    //      0.5,-0.5,-0.5,  0.0,0.0,-1.0,  0.0,1.0, -1.0,0.0,0.0,
    //     -0.5,-0.5,-0.5,  0.0,0.0,-1.0,  1.0,1.0, -1.0,0.0,0.0,

    //     // Left
    //     -0.5, 0.5,-0.5,  -1.0,0.0,0.0,  0.0,0.0,  0.0,0.0,1.0,
    //     -0.5, 0.5, 0.5,  -1.0,0.0,0.0,  1.0,0.0,  0.0,0.0,1.0,
    //     -0.5,-0.5,-0.5,  -1.0,0.0,0.0,  0.0,1.0,  0.0,0.0,1.0,
    //     -0.5,-0.5, 0.5,  -1.0,0.0,0.0,  1.0,1.0,  0.0,0.0,1.0,

    //     // Right
    //      0.5, 0.5, 0.5,  1.0,0.0,0.0,   0.0,0.0, 0.0,0.0,-1.0,
    //      0.5, 0.5,-0.5,  1.0,0.0,0.0,   1.0,0.0, 0.0,0.0,-1.0,
    //      0.5,-0.5, 0.5,  1.0,0.0,0.0,   0.0,1.0, 0.0,0.0,-1.0,
    //      0.5,-0.5,-0.5,  1.0,0.0,0.0,   1.0,1.0, 0.0,0.0,-1.0,

    //     // Top
    //     -0.5, 0.5,-0.5,  0.0, 1.0,0.0,  0.0,0.0,  1.0,0.0,0.0,
    //      0.5, 0.5,-0.5,  0.0, 1.0,0.0,  1.0,0.0,  1.0,0.0,0.0,
    //     -0.5, 0.5, 0.5,  0.0, 1.0,0.0,  0.0,1.0,  1.0,0.0,0.0,
    //      0.5, 0.5, 0.5,  0.0, 1.0,0.0,  1.0,1.0,  1.0,0.0,0.0,

    //     // Bottom
    //      0.5,-0.5,-0.5,  0.0,-1.0,0.0,  0.0,0.0,  1.0,0.0,0.0,
    //     -0.5,-0.5,-0.5,  0.0,-1.0,0.0,  1.0,0.0,  1.0,0.0,0.0,
    //      0.5,-0.5, 0.5,  0.0,-1.0,0.0,  0.0,1.0,  1.0,0.0,0.0,
    //     -0.5,-0.5, 0.5,  0.0,-1.0,0.0,  1.0,1.0,  1.0,0.0,0.0
    // };

    // cube.bindVertices3D(verticesCube, sizeof(verticesCube), indicesCube, sizeof(indicesCube));
    // square.bindVertices2D(verticesSquare, sizeof(verticesSquare), indicesSquare, sizeof(indicesSquare));

    // cubeNormalMapped.bindVertices3D_Normal(verticesCubeNormalMapped, sizeof(verticesCubeNormalMapped), indicesCube, sizeof(indicesCube));

    // cube.center = glm::vec3(0.0f);
    // cube.radius = 0.5f;

    // square.center = glm::vec3(0.0f);
    // square.radius = 0.5f;

    // cubeNormalMapped.center = glm::vec3(0.0f);
    // cubeNormalMapped.radius = 0.5f;

    cube.loadMesh3D(vertices, indices);
    square.loadMesh2D(verticesSqr, indicesSqr);
    cubeNorm.loadMesh3DNormal(verticesNorm, indices);

    // BoundingSphere
    cube.radius     = sqrt(0.75);
    square.radius   = sqrt(0.5);
    cubeNorm.radius = sqrt(0.75);

    // AABB
    cube.minCorner = glm::vec3(-0.5f);
    cube.maxCorner = glm::vec3( 0.5f);

    square.minCorner = glm::vec3(-0.5f,-0.5f, 0.0f);
    square.maxCorner = glm::vec3( 0.5f, 0.5f, 0.0f);

    cubeNorm.minCorner = glm::vec3(-0.5f);
    cubeNorm.maxCorner = glm::vec3( 0.5f);

    // Instance Cubes
    glm::vec3 cubePositions[] = {

        glm::vec3( 5.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f,-15.0f),
        glm::vec3(-1.5f,-2.2f,-2.5f),
        glm::vec3(-3.8f,-2.0f,-12.3f),
        glm::vec3( 2.4f,-0.4f,-3.5f),
        glm::vec3(-1.7f, 3.0f,-7.5f),
        glm::vec3( 1.3f,-2.0f,-2.5f),
        glm::vec3( 1.5f, 2.0f,-2.5f),
        glm::vec3( 1.5f, 0.2f,-1.5f),
        glm::vec3(-1.3f, 1.0f,-1.5f)
    };

    cubes.count = 10;
    cubes.mesh.minCorner = glm::vec3(-0.5f);
    cubes.mesh.maxCorner = glm::vec3( 0.5f);
    
    cubes.transforms.resize(cubes.count);
    cubes.AABBs.resize(cubes.count);

    for (uint32_t i = 0; i < 10; i++) {
        
        cubes.transforms[i].position = cubePositions[i];
        cubes.transforms[i].rotation = (i * 12.0f * glm::vec3(1.0f, 2.0f, 3.0f));
        cubes.transforms[i].scale    = glm::vec3(0.8);
        
        cubes.transforms[i].computeModel();

        cubes.AABBs[i] = BoundingAABBComponent {
            cubes.transforms[i].position,
            cubes.mesh.minCorner,
            cubes.mesh.maxCorner
        };

        cubes.AABBs[i].recompute(cubes.transforms[i].model);
    }

    cubes.bind(vertices, indices);
}

void ECS::destroy(const Entity& entity) {

    if (entity > entityManager.lastEntity) {
        std::cerr << "ERROR::DESTROYING ENTITY WHICH WAS NEVER CREATED!" << std::endl;
        return;
    }

    componentManager.removeComponent<TransformComponent>     (entity);
    componentManager.removeComponent<MeshComponent>          (entity);
    componentManager.removeComponent<MaterialComponent>      (entity);
    componentManager.removeComponent<InstanceComponent>      (entity);
    componentManager.removeComponent<ModelComponent>         (entity);
    componentManager.removeComponent<PointLightComponent>    (entity);
    componentManager.removeComponent<DirectionalLight>       (entity);
    componentManager.removeComponent<BoundingSphereComponent>(entity);
    componentManager.removeComponent<BoundingAABBComponent>  (entity);
    componentManager.removeComponent<MeshLODComponent>       (entity);
    componentManager.removeComponent<ModelLODComponent>      (entity);
    componentManager.removeComponent<CameraComponent>        (entity);
    componentManager.removeComponent<ChildComponent>         (entity);

    if (entity < entityManager.lastEntity) {

        componentManager.copy<TransformComponent>     (entityManager.lastEntity, entity);
        componentManager.copy<MeshComponent>          (entityManager.lastEntity, entity);
        componentManager.copy<MaterialComponent>      (entityManager.lastEntity, entity);
        componentManager.copy<InstanceComponent>      (entityManager.lastEntity, entity);
        componentManager.copy<ModelComponent>         (entityManager.lastEntity, entity);
        componentManager.copy<PointLightComponent>    (entityManager.lastEntity, entity);
        componentManager.copy<DirectionalLight>       (entityManager.lastEntity, entity);
        componentManager.copy<BoundingSphereComponent>(entityManager.lastEntity, entity);
        componentManager.copy<BoundingAABBComponent>  (entityManager.lastEntity, entity);
        componentManager.copy<MeshLODComponent>       (entityManager.lastEntity, entity);
        componentManager.copy<ModelLODComponent>      (entityManager.lastEntity, entity);
        componentManager.copy<CameraComponent>        (entityManager.lastEntity, entity);
        componentManager.copy<ChildComponent>         (entityManager.lastEntity, entity);
    }

    entityManager.nextEntity = entityManager.lastEntity;
}