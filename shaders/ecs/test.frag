#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

void main() {

    FragColor = vec4(1.0, 1.0, 0.0, 1.0); // Red-Color
}