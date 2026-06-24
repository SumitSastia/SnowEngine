#pragma once

#include <ecs/component.h>

struct FlatColorMaterial {

    glm::vec4 albedo    = glm::vec4(1.0f);
    float     metallic  = 0.0f;
    float     roughness = 1.0f;
};

struct PBRMaterial {

    TextureHandle albedo    = 0;
    TextureHandle normal    = 0;
    TextureHandle metallic  = 0;
    TextureHandle roughness = 0;
    TextureHandle ao        = 0;
};

struct PhongMaterial {

    TextureHandle albedo   = 0;
    TextureHandle normal   = 0;
    TextureHandle specular = 0;
};