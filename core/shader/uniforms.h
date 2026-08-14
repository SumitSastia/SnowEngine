#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "core/config.h"
#include "ecs/entity.h"

struct CameraData {

    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 camPos;
    float padding1;
};

struct ScreenData {

    glm::uvec4 screenSize;
};

namespace gfx::internal {

    struct PointLight_ubo {

        glm::vec3 position;
        float constant;

        glm::vec3 color;
        float linear;

        float padding[3];
        float quadratic;
    };
};

struct PointLightData {

    gfx::internal::PointLight_ubo lights[MAX_LIGHTS];

    float padding[3];
    uint  lightCount;
};

class UBOhandler {

public:

    void init();
    void update();

private:

    GLuint cameraUBO;
    GLuint screenUBO;
    CameraData cameraDataBuffer;
};

class PointLightUBO {

public:

    void init(const ECS& ecs);
    void update(const ECS& ecs);

private:

    GLuint UBO;
    PointLightData data;
};

struct ComputeData {
    unsigned int dataSize;
};

class SSBO {

public:

    SSBO() { init(); }
    void init();

    const GLuint getSSBO() const { return ssbo; }

private:
    GLuint ssbo;
};