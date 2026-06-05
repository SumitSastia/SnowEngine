#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

uniform sampler2D albedo;

void main() {

    vec3 color = texture(albedo, vTexCords).rgb;
    FragColor = vec4(color, 1.0);
}