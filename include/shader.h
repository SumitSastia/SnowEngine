#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

// ------------------------------ Function Delcarations ------------------------------ //



// ------------------------------ Classes -------------------------------------------- //

class texture2D {

    int width;
    int height;
    int nrChannels;
    
    std::string type;
    unsigned int textureID;
    unsigned char* pixelData;
    
public:

    void load(const char* path);
    void destroy();

    const unsigned int& getID() const { return textureID; }
    const std::string& getType() const { return type; }
};