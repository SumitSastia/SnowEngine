#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

// ------------------------------ Function Delcarations ------------------------------ //

std::string loadShaderFile(const char* path);
const unsigned int createShader(const char* vertPath, const char* fragPath);
const unsigned int createShader2(const char* vertPath, const char* geomPath, const char* fragPath);

void renderShadow(const u_int& VAO, const u_int& indicesCount);
void renderShadowInstanced(const u_int& VAO, const u_int& indicesCount, const u_int& instanceCounts);

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