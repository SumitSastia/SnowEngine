#pragma once

#include <renderer.h>

inline float cursor_dx = 0.0f;
inline float cursor_dy = 0.0f;

inline bool mouseInCamera = true;

// ------------------------------ Function Delcarations ------------------------------ //

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// For General purpose Mouse-Tracking
void track_mouse(GLFWwindow* window);