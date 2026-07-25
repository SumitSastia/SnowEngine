#pragma once

#include <glad/glad.h>

class ComputeShader {

    GLuint computeShader;
    GLuint shaderProgram;

public:

    void init(const char* path);
    void use() const;
};