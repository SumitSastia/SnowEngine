#version 450 core

out vec4 FragColor;

in vec4 color;
in vec2 vTexCords;

uniform sampler2D particle;

void main() {
    
    const vec4 tex = texture(particle, vTexCords);

    const float gamma = 2.3;
    vec3 finalColor = pow(tex.rgb, vec3(1.0 / gamma));   // Gamma Correction

    FragColor = vec4(finalColor.rgb, color.a * tex.a);
}