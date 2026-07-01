#version 450 core

out vec4 FragColor;
in  vec2 vTexCords;

// uniform vec4 color;
uniform sampler2D particle;

void main() {
    
    // const vec3 tex = texture(particle, vTexCords).rgb;
    const vec4 tex = texture(particle, vTexCords);
    
    // if (tex == vec3(0.0)) discard;

    // if (tex.r == 0.0 && tex.g == 0.0 && tex.b == 0.0) discard;

    // FragColor = vec4(tex, 1.0f);
    FragColor = tex;
    // FragColor = color;
}