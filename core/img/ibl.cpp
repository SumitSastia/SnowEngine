#include <iostream>

#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ibl.h"
#include "cubemap.h"
#include "core/shader/shader.h"
#include "core/renderer.h"
#include "core/frame/frame.h"
#include "utils/assert.h"

IBLFrame::IBLFrame(const char* path, const uint16_t resolution): isInit(false) {

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &env_cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubeMap);

    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    loadEnvironment(path);
    compileEnvironment();

    convertCubeMap(resolution);
    brdfLUT();
    
    isInit = true;
}

void IBLFrame::renderCube() const {

    glBindVertexArray(gfx::cubemap::Cube::getVAO());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void IBLFrame::renderSquare() const {

    frameBuffers::renderScreen();
}

void IBLFrame::loadEnvironment(const char* path) {

    isHDR = true;

    const std::string finalPath = "../" + std::string(path);

    pixelData = stbi_loadf(finalPath.c_str(), &width, &height, nullptr, 4);

    if (!pixelData) {
        std::cerr << "Failed to Load Image!\n" << finalPath << std::endl;
        return;
    }
}

void IBLFrame::compileEnvironment() {

    SNOW_ASSERT(pixelData, "PIXELDATA IS NOT LOADED FROM HDR FILE!");

    glGenTextures(1, &env_texture);
    glBindTexture(GL_TEXTURE_2D, env_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, pixelData);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void IBLFrame::convertCubeMap(const uint16_t resolution) {

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    glm::mat4 captureViews[] = {

        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glViewport(0, 0, resolution, resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    
    const Shader& shader = ShaderManager::getFrame(gfx::shader::EQUIRECT_TO_CUBEMAP);
    shader.use();

    shader.setMat4("projection", captureProjection);
    shader.setInt("equirectangularMap", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, env_texture);

    Renderer::disableCulling();
    Renderer::disableDepth();

    for (unsigned int i = 0; i < 6; i++) {

        shader.setMat4("view", captureViews[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_cubeMap, 0);
        // glClearColor( (i%2)? 0.0 : 0.5, 0.5, (i%2)? 0.5 : 0.0, 1.0);
        glClearColor(0.2, 0.8, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->renderCube();
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->createIrradiance(captureProjection, captureViews, 32);
    this->preFilter(captureProjection, captureViews);
}

void IBLFrame::createIrradiance(const glm::mat4 captureProjection, const glm::mat4 captureViews[], const uint16_t resolution) {

    glViewport(0, 0, resolution, resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);

    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const Shader& shaderBlur = ShaderManager::getFrame(gfx::shader::BLUR_CUBEMAP);

    shaderBlur.use();
    shaderBlur.setMat4("projection", captureProjection);
    shaderBlur.setInt("environmentMap", 0);

    this->bindEnv(0);

    Renderer::disableCulling();
    Renderer::disableDepth();

    for (unsigned int i = 0; i < 6; i++) {

        shaderBlur.setMat4("view", captureViews[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->renderCube();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLFrame::preFilter(const glm::mat4 captureProjection, const glm::mat4 captureViews[]) {

    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

    for (uint i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    const Shader& shaderPrefilter = ShaderManager::getFrame(gfx::shader::PREFILTER_CUBEMAP);

    shaderPrefilter.use();
    shaderPrefilter.setMat4("projection", captureProjection);

    this->bindEnv(0);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    unsigned int maxMipLevels = 5;

    for (unsigned int mip = 0; mip < maxMipLevels; mip++) {

        unsigned int mipWidth  = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);

        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shaderPrefilter.setFloat("roughness", roughness);

        for (unsigned int i = 0; i < 6; i++) {

            shaderPrefilter.setMat4("view", captureViews[i]);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            this->renderCube();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLFrame::brdfLUT() {

    glGenTextures(1, &brdfLUTTexture);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    Renderer::disableDepth();

    glViewport(0, 0, 512, 512);

    ShaderManager::getFrame(gfx::shader::BRDF).use();
    glClearColor(1.0, 1.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->renderSquare();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLFrame::bindEnv(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, env_cubeMap);
}

void IBLFrame::bindIrradianceMap(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
}

void IBLFrame::bindPreFilterMap(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
}

void IBLFrame::bindBRDFLUT(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
}

void IBLFrame::draw(const unsigned int textureUnit) const {

    if (!isInit) return;

    this->bindEnv(textureUnit);
    gfx::cubemap::Cube::draw();
}