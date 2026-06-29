#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <random>

class Shader;

class SSAO {

    static std::vector <glm::vec3> ssaoKernel;
    static std::vector <glm::vec3> ssaoNoise;

    static unsigned int fbo;
    static unsigned int colorBuffer;
    static unsigned int noiseTexture;

    static unsigned int fbo_blur;
    static unsigned int colorBuffer_blur;

    static Shader* shaderBlur;

    static float lerp(float a, float b, float f) {
        return a + f * (b - a);
    }

public:

    static bool enable;

    static Shader* shader;

    static void init();
    static void bindFBO();
    static void bindNoiseTex(const unsigned int textureUnit);
    static void bindOcclusion(const unsigned int textureUnit);

    static void setInt(const unsigned int textureUnit);
    static void blurSSAO();
};