#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <s_time.h>

class DebugMenu {

    struct TextureDataPair {
        std::string object;
        int64_t     init_time;
        uint32_t    handle;
    };

    struct ColorDataPair {
        int64_t init_time;
        uint32_t color;
        uint32_t handle;
    };

    static std::vector <TextureDataPair> textureInit_data;
    static std::vector <ColorDataPair>   colorInit_data;

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

    static void addAssetInitTime(const std::string& object, const int64_t init_time, const uint32_t handle) {

        textureInit_data.push_back({
            object,
            init_time,
            handle
        });
    }

    static void addAssetInitTime(const int64_t init_time, const uint32_t color, const uint32_t handle) {

        colorInit_data.push_back({
            init_time,
            color,
            handle
        });
    }

    static void printAssetsInitTime();

    static void renderTextbox(const std::string title, const std::string desc);
    static void destroy();
};