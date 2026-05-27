#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>

class Shader;

class IBLFrame {

    unsigned int captureFBO, captureRBO;

    unsigned int env_texture;   // Equirectangular Environment 2D
    unsigned int env_cubeMap;   // Environment CubeMap
    unsigned int irradianceMap; // Blurred Environment CubeMap

    Shader* shader;     // Converts 2D -> CubeMap 
    Shader* shaderBlur; // CubeMap -> Blurred CubeMap

    bool isHDR;

    void renderCube() const;
    
    /* Loads the environment that has HDR lighting (.hdr file) */
    void loadEnvironment(const char* path, const uint16_t resolution);
    
    /* @note Call only once to initialize cubemap texture of the Environment. */
    void convertCubeMap(const uint16_t resolution);

    /* @note Call only once to create Irradiance Map of the Environmnet Cubemap. */
    void createIrradiance(glm::mat4 captureProjection, glm::mat4 captureViews[], const uint16_t resolution);

public:

    IBLFrame(const char* path, const uint16_t resolution = 512);

    void bindEnv(const unsigned int textureUnit) const;
    void bindEnvBlurred(const unsigned int textureUnit) const;
};