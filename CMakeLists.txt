cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME SnowEngine)
set(CMAKE_BUILD_TYPE Debug)

project(${TARGET_NAME})

set(
    IMGUI_SRC

    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_impl_glfw.cpp
    include/imgui/imgui_impl_opengl3.cpp
)

set(
    ECS_SRC

    src/ecs/entity.cpp
    src/ecs/component.cpp
    src/ecs/rendersystem.cpp
)

set(
    SCENES_SRC

    src/scenes/scene1.cpp
    src/scenes/scene2.cpp
)

add_executable(
    ${TARGET_NAME}

    src/glad.c
    src/s_math.cpp
    ${IMGUI_SRC}
    src/main.cpp
    src/shader.cpp
    src/camera.cpp
    src/shapes.cpp
    src/renderer.cpp
    src/debug.cpp
    src/input.cpp
    src/entity.cpp
    src/lights.cpp
    src/frame.cpp
    src/model.cpp
    src/ibl.cpp
    src/text.cpp
    src/ssao.cpp
    ${SCENES_SRC}
    ${ECS_SRC}
)

target_include_directories(${TARGET_NAME} PRIVATE include)
target_include_directories(${TARGET_NAME} PRIVATE include/freetype2)

target_link_libraries(${TARGET_NAME} PRIVATE glfw assimp freetype)