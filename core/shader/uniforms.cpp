#include "uniforms.h"
#include "core/camera.h"

const uint bindingPoint_camera  = 0;
const uint bindingPoint_plights = 1;
const uint bindingPoint_screen  = 2;

void UBOhandler::init() {
    
    cameraDataBuffer.projection = Camera::activeCamera->getProjection();
    cameraDataBuffer.view       = Camera::activeCamera->getView();
    cameraDataBuffer.camPos     = glm::vec4(Camera::activeCamera->getPos(), 1.0f);

    glGenBuffers(1, &cameraUBO);
    glGenBuffers(1, &screenUBO);

    // ---------------------------------------------------------------------- //

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);

    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(CameraData),
        &cameraDataBuffer,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint_camera, cameraUBO);

    // ---------------------------------------------------------------------- //

    ScreenData screenData;
    screenData.screenSize = {

        WIN_W,
        WIN_H,
        0,
        0
    };

    glBindBuffer(GL_UNIFORM_BUFFER, screenUBO);

    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(ScreenData),
        &screenData,
        GL_STATIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint_screen, screenUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBOhandler::update() {

    cameraDataBuffer.projection = Camera::activeCamera->getProjection();
    cameraDataBuffer.view       = Camera::activeCamera->getView();
    cameraDataBuffer.camPos     = glm::vec4(Camera::activeCamera->getPos(), 1.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        0,
        sizeof(CameraData),
        &cameraDataBuffer
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void PointLightUBO::init(const ECS& ecs) {

    // Data Initialization
    int counter = 0;

    for (const Entity& entity : ecs.entityManager.emissiveEntities) {

        if (ecs.componentManager.has<PointLightComponent>(entity)) {

            gfx::internal::PointLight_ubo plight;
            
            const auto& plComponent = ecs.componentManager.get<PointLightComponent>(entity);
            plight.position = glm::vec4(ecs.componentManager.get<TransformComponent>(entity).position, 1.0f);
            
            plight.color     = glm::vec4(plComponent.color, 1.0f);
            plight.constant  = plComponent.constant;
            plight.linear    = plComponent.linear;
            plight.quadratic = plComponent.quadratic;

            data.lights[counter++] = plight;
        }
    }

    data.lightCount = counter;

    // Memory Initialization/Binding
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);

    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(PointLightData),
        &data,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint_plights, UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void PointLightUBO::update(const ECS& ecs) {

    // Updated Data
    uint counter = 0;

    for (const Entity& entity : ecs.entityManager.emissiveEntities) {

        if (ecs.componentManager.has<PointLightComponent>(entity)) {

            gfx::internal::PointLight_ubo plight;
            
            const auto& plComponent = ecs.componentManager.get<PointLightComponent>(entity);
            plight.position = glm::vec4(ecs.componentManager.get<TransformComponent>(entity).position, 1.0f);
            
            plight.color     = glm::vec4(plComponent.color, 1.0f);
            plight.constant  = plComponent.constant;
            plight.linear    = plComponent.linear;
            plight.quadratic = plComponent.quadratic;

            data.lights[counter++] = plight;
        }
    }

    data.lightCount = counter;

    // Updating Data inside VRAM
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        0,
        sizeof(PointLightData),
        &data
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// --------------------------------------- SSBO --------------------------------------- //

#include "math/random.h"

void SSBO::init() {

    uint32_t count = 2048;
    size_t size = sizeof(uint32_t) + sizeof(float) * count;

    float numbers[count];

    for (uint i = 0; i < count; i++) numbers[i] = i / count;

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        size,
        nullptr,
        GL_DYNAMIC_DRAW
    );


    glBufferSubData(
        GL_SHADER_STORAGE_BUFFER,
        0,
        sizeof(uint32_t),
        &count
    );

    glBufferSubData(
        GL_SHADER_STORAGE_BUFFER,
        sizeof(uint32_t),
        sizeof(float) * count,
        numbers
    );

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}