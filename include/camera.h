#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace gfx::internal {

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct BoundingSphere {
        glm::vec3 center;
        float radius;
    };

    struct Plane {
        glm::vec3 normal;
        float distance;
    };
};

class Frustum {

    // 0 - Left
    // 1 - Right
    // 2 - Top
    // 3 - Bottom
    // 4 - Near
    // 5 - Far

    std::vector <gfx::internal::Plane> faces;

    void init(const glm::mat4& viewProj);

public:

    Frustum(const glm::mat4& viewProj):
        faces(6)
    {
        init(viewProj);
    }

    bool isMeshInside(const gfx::internal::BoundingSphere& sphere);

    // Extracts 6 Faces(Planes) of the Frustum
    void update(const glm::mat4& viewProj);
};

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
    float camSensitivity;

    float yaw; // y-axis rotation
    float yaw_initial;

    float pitch; // x-axis rotation
    float pitch_initial;

    float fov;
    float aspectRatio;

    bool mouseEnabled;

    // Player Movements
    bool Uturn;

    Frustum* frustum;

public:

    Camera();
    static Camera& instance();

    void update(const float& delta_time);
    void look_at();
    void input_handler(GLFWwindow* window, float deltaTime);
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

    bool isMeshInside(const gfx::internal::BoundingSphere& sphere) {
        return frustum->isMeshInside(sphere);
    }
};


