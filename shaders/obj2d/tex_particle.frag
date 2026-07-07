#version 450 core

out vec4 FragColor;
in  vec2 vTexCords;

// uniform vec4 color;
uniform sampler2D particle;
uniform float alpha;

void main() {
    
    const vec4 tex = texture(particle, vTexCords);

    const float gamma = 2.3;
    vec3 finalColor = pow(tex.rgb, vec3(1.0 / gamma));   // Gamma Correction

    FragColor = vec4(finalColor.rgb, alpha * tex.a);
}