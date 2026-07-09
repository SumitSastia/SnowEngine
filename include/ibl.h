#pragma once

#include <glm/glm.hpp>

#include <cstdint>

class IBLFrame {

    int width, height;

    uint captureFBO, captureRBO;

    uint env_texture;   // Equirectangular Environment 2D
    uint env_cubeMap;   // Environment CubeMap
    uint irradianceMap; // Blurred Environment CubeMap

    uint prefilterMap;
    uint brdfLUTTexture; 

    // Shader shader;          // Converts 2D -> CubeMap 
    // Shader shaderBlur;      // CubeMap -> Blurred CubeMap
    // Shader shaderPrefilter; // Prefilters CubeMap
    // Shader shaderBRDF;

    bool isHDR;
    bool isInit;

    float* pixelData;

    void renderCube() const;
    void renderSquare() const;
    
    /* Loads the environment that has HDR lighting (.hdr file) */
    // void loadEnvironment(const char* path, const uint16_t resolution);
    void loadEnvironment(const char* path);
    void compileEnvironment();
    
    /* @note (For Diffuse IBL) Call only once to initialize cubemap texture of the Environment. */
    void convertCubeMap(const uint16_t resolution);

    /* @note (For Diffuse IBL) Call only once to create Irradiance Map of the Environmnet Cubemap. */
    void createIrradiance(const glm::mat4 captureProjection, const glm::mat4 captureViews[], const uint16_t resolution);

    /* @note (For Specular IBL) Call only once to generate Pre-Filtered Map. */
    void preFilter(const glm::mat4 captureProjection, const glm::mat4 captureViews[]);

    /* @note (For Specular IBL) Call only once */
    void brdfLUT();

public:

    IBLFrame(const char* path, const uint16_t resolution = 512);

    void bindEnv(const unsigned int textureUnit) const;
    void bindIrradianceMap(const unsigned int textureUnit) const;
    void bindPreFilterMap(const unsigned int textureUnit) const;
    void bindBRDFLUT(const unsigned int textureUnit) const;

    void draw(const unsigned int textureUnit) const;
};