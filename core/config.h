#pragma once

const unsigned int WIN_W = 1280u;
const unsigned int WIN_H = 720u;

using TextureHandle = uint32_t;
using ShaderHandle  = uint32_t;

using UintRes = uint16_t;

// WARNING: SHOULD BE SAME AS THAT DEFINED IN GLSL SHADER
#define MAX_LIGHTS 4U