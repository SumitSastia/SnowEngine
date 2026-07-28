cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME SnowEngine)
set(CMAKE_BUILD_TYPE Debug)

project(${TARGET_NAME})

set(
    MATH_SRC

    math/matrix.cpp
    math/random.cpp
)

set(
    CORE_SRC

    main/glad.c
    main/main.cpp

    core/camera.cpp
    core/renderer.cpp
    core/input.cpp
    core/lights.cpp
    core/shader.cpp
    core/frustum.cpp
    core/text.cpp

    core/frame/frame.cpp
    core/model/model.cpp
    core/deferred/ssao.cpp

    core/shader/uniforms.cpp
    core/shader/compute.cpp

    core/img/assetManager.cpp
    core/img/cubemap.cpp
    core/img/ibl.cpp
    core/img/texture.cpp
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

    ecs/entity.cpp
    ecs/component.cpp
)

set(
    SCENES_SRC

    scenes/scene1.cpp
    scenes/scene2.cpp
)

set(
    ECS_SYSTEMS

    ecs/systems/initSystem.cpp
    ecs/systems/updateSystem.cpp
    ecs/systems/renderSystem.cpp
)

add_executable(
    ${TARGET_NAME}

    ${MATH_SRC}
    ${CORE_SRC}

    utils/debug.cpp
    utils/wireframe.cpp
    utils/crosshair.cpp
    core/gfx/particles.cpp
    
    ${SCENES_SRC}
    ${ECS_SRC}
    ${IMGUI_SRC}
    ${ECS_SYSTEMS}
)

target_include_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE include)
target_include_directories(${TARGET_NAME} PRIVATE include/freetype2)

# target_link_libraries(${TARGET_NAME} PRIVATE glfw assimp freetype)

if(WIN32)

    target_link_directories(${TARGET_NAME} PRIVATE libs)
    target_link_libraries(${TARGET_NAME} PRIVATE glfw3 opengl32 assimp-vc143-mt freetype)

    add_custom_command(TARGET SnowEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/libs/assimp-vc143-mt.dll"
            $<TARGET_FILE_DIR:SnowEngine>
    )

elseif(UNIX)
    target_link_libraries(${TARGET_NAME} PRIVATE glfw assimp freetype)
endif()