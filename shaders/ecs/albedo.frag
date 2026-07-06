#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

uniform sampler2D albedo;

void main() {

    vec4 color = texture(albedo, vTexCords);
    FragColor = color;
}