#pragma once

#include <s_math.h>

class Shape;

namespace lights {

    struct DirectionalLight {

        glm::vec3 direction; // 12-Bytes
        glm::vec3 color;     // 12-Bytes
    };

    struct PointLight {

        glm::vec3 position;
        glm::vec3 color;    // 12-Bytes

        float constant;     // 4-Bytes 
        float linear;       // 4-Bytes 
        float quadratic;    // 4-Bytes 
    };

    struct SpotLight {

        glm::vec3 position;  // 12-Bytes
        glm::vec3 direction; // 12-Bytes
        glm::vec3 color;     // 12-Bytes

        float cutOffangle;   // 4-Bytes
        float outerCutOff;   // 4-Bytes
        
        float constant;      // 4-Bytes
        float linear;        // 4-Bytes
        float quadratic;     // 4-Bytes
        
        bool isVisible;      // 1-Byte
    };
}

struct DefaultLights {

    lights::PointLight       cubelight;
    lights::DirectionalLight sunlight;
    lights::SpotLight        flashlight;

    DefaultLights();

    static DefaultLights& instance() {
        static DefaultLights instance {};
        return instance;
    }

    void update();
};

class LightSource {

    lights::PointLight src;
    Shape* srcShape;

public:

    LightSource();

    void setLightColor(const glm::vec3& color)    { src.color = color; }
    void setPosition  (const glm::vec3& position) { src.position = position; }

    void setX(const float& x);
    void setY(const float& y);
    void setZ(const float& z);

    glm::vec3 getLightColor() const { return src.color; }
    glm::vec3 getPosition()   const { return src.position; }

    glm::mat4 getModel() const { return glm::translate(glm::mat4(1.0f), src.position); }
    const lights::PointLight& getPointLight() const { return src; }

    void draw() const;
};