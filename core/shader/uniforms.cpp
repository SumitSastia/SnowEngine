#include "uniforms.h"
#include "core/camera.h"

const uint bindingPoint_camera  = 0;
const uint bindingPoint_plights = 1;

void UBOhandler::init() {
    
    cameraDataBuffer.projection = Camera::activeCamera->getProjection();
    cameraDataBuffer.view       = Camera::activeCamera->getView();
    cameraDataBuffer.camPos     = glm::vec4(Camera::activeCamera->getPos(), 1.0f);

    // ---------------------------------------------------------------------- //

    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);

    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(CameraData),
        &cameraDataBuffer,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint_camera, cameraUBO);
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