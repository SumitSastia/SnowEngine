#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;

uniform sampler2D texture0;

void main() {

    vec3 color = texture(texture0, vTexCords).rgb;

    FragColor = vec4(color, 1.0);
}