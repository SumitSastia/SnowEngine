#version 450 core

out vec4 FragColor;
in vec2 vTexCords;

layout (std430, binding = 0) buffer Data {

    uint dataSize;
    float numbers[];
};

void main() {

    FragColor = vec4(vec3(numbers[0]), 1.0);
    // FragColor = vec4(vec3(0.5), 1.0);
}