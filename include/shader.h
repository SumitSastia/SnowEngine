#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

const uint8_t MAX_SHADERS = 8;

// ------------------------------ Function Delcarations ------------------------------ //



// ------------------------------ Classes -------------------------------------------- //

class Shader {

    unsigned int shaderProgram;

public:

    Shader(const char* vertPath, const char* fragPath);
    Shader(const char* vertPath, const char* geomPath, const char* fragPath);

    const unsigned int getShader() const { return shaderProgram; }

    void use() const;
    void destroy();
};

class Texture2D {

    int width;
    int height;
    int nrChannels;
    
    std::string type;
    unsigned int textureID;
    unsigned char* pixelData;
    
public:

    /*
    Loads the image and allocate it into the Memory.
    NOTE: "path" should start with '/' and rest should continue after the Main Directory (/SnowEngine).
    */
    void load(const char* path);

    void destroy();

    const unsigned int& getID() const { return textureID; }
    const std::string& getType() const { return type; }
};