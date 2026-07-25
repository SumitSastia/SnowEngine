#include "compute.h"
#include "core/shader.h"

#include <iostream>

void ComputeShader::init(const char* path) {

    const std::string shaderStr = gfx::shader::loadShaderFile("../shaders/compute/particle.glsl");
    const char* shaderSource = shaderStr.c_str();

    computeShader = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(computeShader, 1, &shaderSource, nullptr);
    glCompileShader(computeShader);

    int success;
    char infoLog[512];

    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(computeShader, 512, nullptr, infoLog);
        std::cerr << "ERROR: VERTEX-SHADER COMPILATION FAILED!\n" << infoLog << std::endl;
        std::abort();
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, computeShader);
    glLinkProgram(shaderProgram);
}

void ComputeShader::use() const {
    glUseProgram(shaderProgram);
}