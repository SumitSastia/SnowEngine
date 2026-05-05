#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <renderer.h>

class debug_menu {


public:

    static debug_menu& instance() {
        static debug_menu instance{};
        return instance;
    }

    void init(GLFWwindow* window);
    void update();
    void render() const;
    void destroy();
};