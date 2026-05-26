#pragma once

class Shader;

class IBLFrame {

    unsigned int captureFBO, captureRBO;
    unsigned int env_texture, env_cubeMap;
    Shader* shader;

    bool isHDR;

    void renderCube() const;

public:

    IBLFrame();

    /* Loads the environment that has HDR lighting (.hdr file) */
    void loadEnvironment(const char* path);

    /* @note Call only once to initialize cubemap texture of the Environment. */
    void convertCubeMap() const;

    void bindEnv(const unsigned int textureUnit) const;
};