#version 450 core

out vec4 FragColor;
in vec2 vTexCords;

layout (std140, binding = 2) uniform Screen {

    uvec4 screenSize;
};

layout (std430, binding = 0) buffer Data {

    uint dataSize;
    float numbers[];
};

uniform sampler2D computeTexture;

void main() {

    uint index = uint(gl_FragCoord.y) * screenSize.x + uint(gl_FragCoord.x);
    index = index % dataSize;

    vec3 color = vec3(0.0);

    if (index > 2047) {
        color = vec3(1.0, 0.0, 1.0);
    }
    else {
        color = vec3(numbers[index]);
    }

    // FragColor = vec4(color, 1.0);
    FragColor = vec4(texture(computeTexture, vTexCords).rgb, 1.0);
}