#include <input.h>

#include <iostream>
#include <algorithm>

bool* Input::currentState  = nullptr;
bool* Input::previousState = nullptr;

void Input::init() {

    currentState  = new bool[GLFW_KEY_LAST + 1]();
    previousState = new bool[GLFW_KEY_LAST + 1]();
}

void Input::update() {
    
    std::copy(
        currentState,
        currentState + GLFW_KEY_LAST + 1,
        previousState
    );
}

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key < 0 || key >= GLFW_KEY_LAST) return;

    if (action == GLFW_PRESS) {
        currentState[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        currentState[key] = false;
    }
}

void track_mouse(GLFWwindow* window){

    static bool mouse_click_hold = false;

    if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {

        cursor_dx = 0.0f;
        cursor_dy = 0.0f;
        mouse_click_hold = false;
        return;
    }

    double pos_x;
    double pos_y;

    glfwGetCursorPos(window, &pos_x, &pos_y);

    static double prev_x = pos_x;
    static double prev_y = pos_y;

    if (!mouse_click_hold) {
        prev_x = pos_x;
        prev_y = pos_y;
        mouse_click_hold = true;
    }

    cursor_dx = pos_x - prev_x;
    cursor_dy = pos_y - prev_y;

    prev_x = pos_x;
    prev_y = pos_y;
}

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y) {
    scrollOffset = (float)offset_y;
}