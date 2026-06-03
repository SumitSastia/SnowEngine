#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <s_math.h>
#include <s_time.h>

#include <string>
#include <iostream>

struct TextBox {
        
    std::string title;
    std::string desc;
};

class DebugMenu {

public:

    static DebugMenu& instance() {
        static DebugMenu instance{};
        return instance;
    }

    template <typename T>
    static void log(T data) {
        std::cout << data << '\n';
    }

    static void printVec3(const glm::vec3& vec) {
        std::cout << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z << '\n';
    }

    static void beginUI();
    static void endUI();

    static void beginBox(const char* title) { ImGui::Begin(title); }
    static void endBox() { ImGui::End(); }

    static void init(GLFWwindow* window);
    static void update();
    static void render() {}
    static void renderTextbox(const std::string title, const std::string desc);
    static void destroy();
};