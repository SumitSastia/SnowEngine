#include <camera.h>
#include <debug.h>
#include <renderer.h>
#include <input.h>
#include <lights.h>

#include <iostream>

Camera::Camera() {

    position  = glm::vec3(0.0f, 0.0f, 3.0f);
    target    = glm::vec3(0.0f, 0.0f, -1.0f);
    direction = glm::normalize(position - target);

    camSlow  = 0.5f;
    camSpeed = 1.0f;
    camSensitivity = 0.1f;

    glm::vec3 up(0.0f, 1.0f, 0.0f);

    right_axis = glm::normalize(glm::cross(up, direction));
    up_axis    = glm::cross(direction, right_axis);
    fov        = 45.0f;

    aspectRatio = (float)WIN_W / (float)WIN_H;

    projection  = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
    mouseEnabled = true;

    yaw   = -90.0f;
    pitch = 0.0f;

    // Player Movements
    Uturn = false;
}

Camera& Camera::instance() {
    static Camera instance;
    return instance;
}

void Camera::update(const float& delta_time) {

    if (Uturn) {
        mouseEnabled = false;
        
        if (yaw < yaw_initial + 180.0f) {
            yaw += 2.0f;
        }
        else {
            Uturn = false;
            mouseEnabled = true;
        }
    }

    glm::vec3 new_direction;

    new_direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_direction.y = sin(glm::radians(pitch));
    new_direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    target = glm::normalize(new_direction);

    look_at();

    ImGui::Text("Camera FOV: %.1f", fov);
}

void Camera::set_position(const glm::vec3 position){
    this->position = position;
}

void Camera::set_target(const glm::vec3 target){

    this->target = target;

    const glm::vec3 target_norm = glm::normalize(target);

    pitch = asin(target_norm.y);
    yaw = atan2(target_norm.z, target_norm.x);

    pitch = glm::degrees(pitch);
    yaw = glm::degrees(yaw);
}

void Camera::set_speed(const float speed){
    camSpeed = speed;
}

void Camera::set_fov(const float fov){

    this->fov = fov;

    if(fov > 89.0f) this->fov = 89.0f;
    if(fov < -89.0f) this->fov = -89.0f;

    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::set_aspect(const int frameWidth, const int frameHeight){

    aspectRatio = (float)frameWidth / (float)frameHeight;
    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::set_yaw(const float& yaw) {
    this->yaw = yaw;
}

void Camera::look_at(){
    viewMatrix = glm::lookAt(position, position + target, up_axis);
}

void Camera::input_handler(GLFWwindow* window, float deltaTime){
    
    if (glfwGetKey(window,GLFW_KEY_W)) {
        position += target * camSpeed * deltaTime;
    }
    if (glfwGetKey(window,GLFW_KEY_S)) {
        position -= target * camSpeed * deltaTime;
    }
    
    if (glfwGetKey(window,GLFW_KEY_A)) {
        position -= glm::normalize(glm::cross(target,up_axis)) * camSpeed * deltaTime;
    }
    if (glfwGetKey(window,GLFW_KEY_D)) {
        position += glm::normalize(glm::cross(target,up_axis)) * camSpeed * deltaTime;
    }

    if (glfwGetKey(window,GLFW_KEY_SPACE)) {
        position.y += camSpeed * deltaTime;
    }
    if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)) {
        position.y -= camSpeed * deltaTime;
    }

    if (glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)) {
        camSpeed = 5.0f;
    }
    else{
        camSpeed = 1.0f;
    }

    if (Input::isKeyDown(GLFW_KEY_E)) {
        (mouseEnabled)?
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL) : glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseEnabled = !mouseEnabled;
    }

    if (Input::isKeyDown(GLFW_KEY_X)) {
        Uturn = true;
        yaw_initial = yaw;
    }

    if (Input::isKeyDown(GLFW_KEY_T)) {
        DefaultLights::instance().flashlight.isVisible = !DefaultLights::instance().flashlight.isVisible;
    }
}

void Camera::mouse_handler(GLFWwindow* window){
    
    double pos_x = 0.0;
    double pos_y = 0.0;
    
    glfwGetCursorPos(window, &pos_x, &pos_y);
    
    static double prev_x = pos_x;
    static double prev_y = pos_y;

    if(!mouseEnabled){
        prev_x = pos_x;
        prev_y = pos_y;
        return;
    }

    double cursor_dx = pos_x - prev_x;
    double cursor_dy = prev_y - pos_y;

    prev_x = pos_x;
    prev_y = pos_y;

    float offset_x = cursor_dx * camSensitivity;
    float offset_y = cursor_dy * camSensitivity;

    yaw = glm::mod(yaw+offset_x, 360.0f);
    pitch += offset_y;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
}

void Camera::scroll_handler(float &scrollOffset){
    
    set_fov(fov - scrollOffset);
    scrollOffset = 0.0f;
}