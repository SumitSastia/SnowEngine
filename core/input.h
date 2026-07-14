#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

inline float cursor_dx = 0.0f;
inline float cursor_dy = 0.0f;

inline float scrollOffset = 0.0f;

class Input {

    static bool* currentState;
    static bool* previousState;

    static bool* prev_mousebutton;
    static bool* curr_mousebutton;

public:

    static void init();
    static void update();

    static bool isKeyPressed (const int key) { return  currentState[key];                        }
    static bool isKeyDown    (const int key) { return  currentState[key] && !previousState[key]; }
    static bool isKeyReleased(const int key) { return !currentState[key] &&  previousState[key]; }

    static bool isMouseButtonPressed (const uint32_t button) { return  curr_mousebutton[button]; }
    static bool isMouseButtonDown    (const uint32_t button) { return  curr_mousebutton[button] && !prev_mousebutton[button]; }
    static bool isMouseButtonReleased(const uint32_t button) { return !curr_mousebutton[button] &&  prev_mousebutton[button]; }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButton_callback(GLFWwindow* window, int button, int action, int mods);
};

// ------------------------------ Function Delcarations ------------------------------ //

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y);

// For General purpose Mouse-Tracking
void track_mouse(GLFWwindow* window);