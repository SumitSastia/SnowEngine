#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

inline float cursor_dx = 0.0f;
inline float cursor_dy = 0.0f;

inline float scrollOffset = 0.0f;

inline bool mouseInCamera = true;

// ------------------------------ Function Delcarations ------------------------------ //

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double offset_x, double offset_y);

// For General purpose Mouse-Tracking
void track_mouse(GLFWwindow* window);