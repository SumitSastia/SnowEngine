#include <cubemap.h>
#include <core/globals.h>

#include <stb_image.h>
#include <iostream>

namespace gfx::cubemap {

    unsigned int Cube::vao = 0;
    unsigned int Cube::vbo = 0;

    Cube::Cube() {

        const float cubemapVertices[] = {
            
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
};

CubeMap::CubeMap(const std::vector <std::string>& textureFaces, const uint16_t internal_format, const uint16_t type, const uint16_t res_size) {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    unsigned char* pixelData;
    int width, height, nrChannels;

    const unsigned int size = textureFaces.size();

    std::string base_str = snow::core::base_filesystem_path.string();
    base_str.erase(base_str.size() - 5);

    for (unsigned int i = 0; i < size; i++) {

        std::string finalPath = base_str + textureFaces[i];

        pixelData = stbi_load(
            finalPath.c_str(),
            &width, &height,
            &nrChannels, 0
        );

        if (pixelData) {

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, internal_format, (res_size)? res_size : width, (res_size)? res_size : height,
                0, GL_RGB, type, pixelData
            );
        }
        else {
            std::cerr << "ERROR::FAILED TO LOAD CUBEMAP!" << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::bindTexture(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void CubeMap::destroy() {

    glDeleteTextures(1, &textureID);
}

//-------------------------------------------------------------------------------------//

Environment::Environment(const char* path, const uint16_t resolution) {

    m_cubeMap = nullptr;
    isVisible = true;
    m_irradianceMap = nullptr;

    iblFrame = new IBLFrame(path, resolution);
}

Environment::Environment(const std::vector <std::string>& textureFaces) {

    m_cubeMap  = new CubeMap(textureFaces);
    isVisible  = false;

    // Not auto-generated currently
    m_irradianceMap = nullptr;
    iblFrame        = nullptr;
}

void Environment::setIrradianceMap(const std::vector <std::string>& textureFaces) {

    if (m_irradianceMap) {
    
        m_irradianceMap->destroy();
        delete m_irradianceMap;
    }

    m_irradianceMap = new CubeMap(textureFaces);
}

void Environment::bindTexture(const unsigned int textureUnit) const {
   
   if (m_cubeMap) m_cubeMap->bindTexture(textureUnit);
   if (iblFrame) iblFrame->bindEnv(textureUnit);
}

void Environment::bindIrradiance(const unsigned int textureUnit) const {
    if (m_irradianceMap) m_irradianceMap->bindTexture(textureUnit);
    if (iblFrame) iblFrame->bindIrradianceMap(textureUnit);
}

void Environment::bindPrefilter(const unsigned int textureUnit) const {
    iblFrame->bindPreFilterMap(textureUnit);
}

void Environment::bindBRDF(const unsigned int textureUnit) const {
    iblFrame->bindBRDFLUT(textureUnit);
}

void Environment::draw() const {

    glBindVertexArray(gfx::cubemap::Cube::getVAO());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Environment::destroy() {
    delete m_cubeMap;
}