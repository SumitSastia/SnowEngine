#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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


// ------------------------------ Shader Uniform Setter ------------------------------ //

void setBool(const unsigned int& shaderProgram, const char* target, const bool& value);

void setInt(const unsigned int &shaderProgram, const char* target, const int &value);

void setFloat(const unsigned int &shaderProgram, const char* target, const float &value);

void setVec3(const unsigned int &shaderProgram, const char* target, const glm::vec3 &vector);

void setMat3(const unsigned int &shaderProgram, const char* target, const glm::mat3 &matrix);

void setMat4(const unsigned int &shaderProgram, const char* target, const glm::mat4 &matrix);