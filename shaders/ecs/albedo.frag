#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

uniform sampler2D albedo;

void main() {

    vec4 color = texture(albedo, vTexCords);

    const float gamma = 1.1;
    vec3 finalColor = pow(color.rgb, vec3(1.0 / gamma)); // Gamma Correction

    FragColor = vec4(finalColor, color.a);
}