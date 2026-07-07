cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME SnowEngine)
set(CMAKE_BUILD_TYPE Debug)

project(${TARGET_NAME})

set(
    MATH_SRC

    src/math/matrix.cpp
    src/math/random.cpp
)

set(
    CORE_SRC

    src/core/glad.c
    src/core/main.cpp
    src/core/shader.cpp
    src/core/camera.cpp
    # src/core/shapes.cpp
    src/core/renderer.cpp
    src/core/input.cpp
    src/core/lights.cpp
    src/core/frame.cpp
    src/core/model.cpp
    src/core/ibl.cpp
    src/core/text.cpp
    src/core/ssao.cpp
    src/core/frustum.cpp
    src/core/assetManager.cpp
    src/core/texture.cpp
    src/core/cubemap.cpp
)

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
)

set(
    SCENES_SRC

    src/scenes/scene1.cpp
    src/scenes/scene2.cpp
)

set(
    ECS_SYSTEMS

    src/systems/initSystem.cpp
    src/systems/updateSystem.cpp
    src/systems/renderSystem.cpp
)

add_executable(
    ${TARGET_NAME}

    ${MATH_SRC}
    ${CORE_SRC}
    src/debug/debug.cpp
    src/utils/wireframe.cpp
    src/utils/crosshair.cpp
    src/gfx/particles.cpp
    
    ${SCENES_SRC}
    ${ECS_SRC}
    ${IMGUI_SRC}
    ${ECS_SYSTEMS}
)

target_include_directories(${TARGET_NAME} PRIVATE include)
target_include_directories(${TARGET_NAME} PRIVATE include/freetype2)

target_link_libraries(${TARGET_NAME} PRIVATE glfw assimp freetype)