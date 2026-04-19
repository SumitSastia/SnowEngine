cmake_minimum_required(VERSION 3.10)

set(TARGET_NAME SnowEngine)

project(${TARGET_NAME})

add_executable(${TARGET_NAME} 
    src/glad.c
    src/main.cpp
    src/shader.cpp
    src/camera.cpp
)

target_include_directories(${TARGET_NAME} PRIVATE include)
target_link_libraries(${TARGET_NAME} PRIVATE glfw)