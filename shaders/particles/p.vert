#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCords;

out vec2 vTexCords;

layout (std140, binding = 0) uniform CameraData {

    mat4 projection;
    mat4 view;
    vec3 camPos;
    float padding1;
};

// hello world

uniform mat4 model;

void main() {

    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
    vTexCords = aTexCords;
}