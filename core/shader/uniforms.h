#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "core/config.h"

struct CameraData {

    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 cameraPos;
};

namespace gfx::internal {

    struct PointLight_ubo {

        glm::vec3 position;
        glm::vec3 color;

        float constant;
        float linear;
        float quadratic;
    };
};

struct PointLightData {

    gfx::internal::PointLight_ubo lights[MAX_LIGHTS];
    uint lightCount;
};

class UBOhandler {

public:
    void init();
    void update();

private:

    GLuint cameraUBO;
    GLuint plightsUBO;

    CameraData cameraDataBuffer;
    PointLightData plData;
};
