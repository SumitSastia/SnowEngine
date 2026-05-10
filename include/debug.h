#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Debug_menu {


public:

    static Debug_menu& instance() {
        static Debug_menu instance{};
        return instance;
    }

    void init(GLFWwindow* window);
    void update();
    void render() const;
    void destroy();
};