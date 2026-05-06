#include <input.h>

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if(glfwGetKey(window, GLFW_KEY_ESCAPE)){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if(glfwGetKey(window, GLFW_KEY_E)){

        if(!mouseInCamera) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else               glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        mouseInCamera = !mouseInCamera;
    }
}

void track_mouse(GLFWwindow* window){

    static bool mouse_click_hold = false;

    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {

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

    if(!mouse_click_hold) {
        prev_x = pos_x;
        prev_y = pos_y;
        mouse_click_hold = true;
    }

    cursor_dx = pos_x - prev_x;
    cursor_dy = pos_y - prev_y;

    prev_x = pos_x;
    prev_y = pos_y;
}