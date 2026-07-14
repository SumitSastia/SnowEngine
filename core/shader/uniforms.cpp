#include "uniforms.h"
#include "core/camera.h"

void UBOhandler::init() {

    const uint bindingPoint_camera  = 0;
    const uint bindingPoint_plights = 1;
    
    cameraDataBuffer.projection = Camera::activeCamera->getProjection();
    cameraDataBuffer.view       = Camera::activeCamera->getView();
    cameraDataBuffer.cameraPos  = Camera::activeCamera->getPos();

    plData.lightCount = 0;

    // ---------------------------------------------------------------------- //

    glGenBuffers(1, &cameraUBO);
    glGenBuffers(1, &plightsUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);

    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(CameraData),
        &cameraDataBuffer,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint_camera, cameraUBO);

    // ---------------------------------------------------------------------- //

    glBindBuffer(GL_UNIFORM_BUFFER, plightsUBO);

    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(PointLightData),
        &plData,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint_plights, plightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBOhandler::update() {

    cameraDataBuffer.projection = Camera::activeCamera->getProjection();
    cameraDataBuffer.view       = Camera::activeCamera->getView();
    cameraDataBuffer.cameraPos  = Camera::activeCamera->getPos();
    
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        0,
        sizeof(CameraData),
        &cameraDataBuffer
    );

    glBindBuffer(GL_UNIFORM_BUFFER, plightsUBO);

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        0,
        sizeof(PointLightData),
        &plData
    );

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}