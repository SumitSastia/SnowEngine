#include <ssao.h>
#include <renderer.h>
#include <shader.h>

std::vector <glm::vec3> SSAO::ssaoKernel {};
std::vector <glm::vec3> SSAO::ssaoNoise  {};

unsigned int SSAO::fbo          = 0;
unsigned int SSAO::colorBuffer  = 0;
unsigned int SSAO::noiseTexture = 0;

Shader* SSAO::shader = nullptr;

// ----------------------------------------------------------------------------------- //

void SSAO::init() {

    // Shader
    shader = new Shader(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/ssao/ssao.frag"
    );

    shader->use();

    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (uint8_t i = 0; i < 64; i++) {

        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );

        sample  = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = (float)i / 64.0f;
        
        scale   = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;

        shader->setVec3(("samples[" + std::to_string(i) + "]").c_str(), sample);

        ssaoKernel.push_back(sample); 
    }

    for (uint8_t i = 0; i < 16; i++) {

        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f
        );

        ssaoNoise.push_back(noise);
    }

    // Noise Texture

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // SSAO Frame

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIN_W, WIN_H, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::bindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void SSAO::bindNoiseTex(const unsigned int textureUnit) {

    shader->setInt("texNoise", textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
}

void SSAO::bindOcclusion(const unsigned int textureUnit) {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
}

void SSAO::setInt(const unsigned int textureUnit) {

    shader->use();
}