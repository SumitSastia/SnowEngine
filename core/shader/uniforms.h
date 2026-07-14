#pragma once

#include <glad/glad.h>

class UBOhandler {

public:
    void init();
    void update();

private:
    GLuint cameraUBO;
};