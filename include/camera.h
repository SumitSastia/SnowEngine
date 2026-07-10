#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//***************************************************************************//

class Camera {
    
    glm::mat4 viewMatrix;
    glm::mat4 projection;

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 target;
    
    glm::vec3 right_axis;
    glm::vec3 up_axis;

    float nearPlane;
    float farPlane;

    float camSlow;
    float camSpeed;
    
    float fov;
    float aspectRatio;

    float yaw; // y-axis rotation
    float yaw_initial;
    
    float pitch; // x-axis rotation
    float pitch_initial;
    
    static float camSensitivity;
    static bool  mouseEnabled;

    // Player Movements
    bool Uturn;

public:

    Camera();
    static Camera& instance() {
        static Camera instance {};
        return instance;
    }

    static Camera* activeCamera;

    void input(GLFWwindow* window, const float deltaTime);
    void update(const float deltaTime);
    void handle_mouse(GLFWwindow* window);

    void look_at();
    void mouse_handler(GLFWwindow* window);
    void scroll_handler(float &scrollOffset);

    void set_speed(const float speed);
    void set_position(const glm::vec3 position);
    void set_target(const glm::vec3 target);
    void set_fov(const float fov);
    void set_aspect(const int frameWidth, const int frameHeight);

    glm::vec3 getPos()        const { return position;   }
    glm::mat4 getView()       const { return viewMatrix; }
    glm::mat4 getProjection() const { return projection; }
    glm::vec3 getTarget()     const { return target;     }

    float getNearPlane() const { return nearPlane; }
    float getFarPlane()  const { return farPlane;  }

    const float getPitch() const { return pitch; }
    const float getYaw()   const { return yaw;   }
};


