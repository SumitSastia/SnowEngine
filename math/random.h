#pragma once

#include <random>
#include <glm/glm.hpp>

glm::vec3 randomPosition(float min = -100.0f, float max = 100.0f);

class Random {

    static std::mt19937       random_engine;
    static std::random_device random_device;

public:

    static void init();

    static int       Int  (const int        min, const int        max);
    static float     Float(const float      min, const float      max);
    static glm::vec3 vec3 (const float      min, const float      max);
    static glm::vec4 vec4 (const float      min, const float      max);
    static glm::vec3 vec3 (const glm::vec3& min, const glm::vec3& max);

    static int Int (const int range) {
        if (range > 0) return Int(-range, range);
        return(range, -range);
    }

    static float Float(const float range) {
        if (range > 0.0f) return Float(-range, range);
        return Float(range, -range);
    }

    static glm::vec3 vec3(const float range) {
        if (range > 0.0f) return vec3(-range, range);
        return vec3(range, -range);
    }

    static glm::vec4 vec4(const float range) {
        if (range > 0.0f) return vec4(-range, range);
        return vec4(range, -range);
    }
};