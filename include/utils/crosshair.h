#pragma once

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <vector>
#include <glm/glm.hpp>

class Crosshair {

    static glm::mat4 model1;
    static glm::mat4 model2;

public:    
    
    static void init();
    static void render(const glm::vec3& color = glm::vec3(1.0f));
};