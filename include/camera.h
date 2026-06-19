#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//***************************************************************************//

class Camera {

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 target;

    glm::vec3 right_axis;
    glm::vec3 up_axis;

    glm::mat4 viewMatrix;
    glm::mat4 projection;

    float camSlow;
    float camSpeed;
    
    float yaw; // y-axis rotation
    float yaw_initial;
    
    float pitch; // x-axis rotation
    float pitch_initial;
    
    float fov;
    float aspectRatio;
    
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

    static const glm::vec3& get_position() { return activeCamera->position; }
    static const glm::mat4& get_projection() { return activeCamera->projection; }
    static const glm::mat4& get_view() { return activeCamera->viewMatrix; }

    static void input(GLFWwindow* window, const float deltaTime);
    static void update(const float deltaTime);
    static void handle_mouse(GLFWwindow* window);

    // void update(const float& delta_time);
    void look_at();
    // void input_handler(GLFWwindow* window, float deltaTime);
    void mouse_handler(GLFWwindow* window);
    void scroll_handler(float &scrollOffset);

    void set_speed(const float speed);
    void set_position(const glm::vec3 position);
    void set_target(const glm::vec3 target);
    void set_fov(const float fov);
    void set_aspect(const int frameWidth, const int frameHeight);
    void set_yaw(const float& yaw);

    float get_yaw() const { return yaw; }

    glm::vec3 getPos() const { return position; }
    glm::mat4 getView() const { return viewMatrix; }
    glm::mat4 getPerspective() const { return projection; }
    glm::vec3 getTarget() const { return target; }
};


