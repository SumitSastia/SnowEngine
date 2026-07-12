#version 450 core

out vec4 FragColor;

in vec4 color;
in vec2 vTexCords;

void main() {
    FragColor = color;
}