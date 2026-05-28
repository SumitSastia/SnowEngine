#pragma once

#include <s_math.h>

#include <vector>
#include <random>

class Shader;

class SSAO {

    static std::vector <glm::vec3> ssaoKernel;
    static std::vector <glm::vec3> ssaoNoise;

    static unsigned int fbo;
    static unsigned int colorBuffer;
    static unsigned int noiseTexture;

    static float lerp(float a, float b, float f) {
        return a + f * (b - a);
    }

public:
    
    static Shader* shader;

    static void init();
    static void bindFBO();
    static void bindNoiseTex(const unsigned int textureUnit);
    static void bindOcclusion(const unsigned int textureUnit);

    static void setInt(const unsigned int textureUnit);
};