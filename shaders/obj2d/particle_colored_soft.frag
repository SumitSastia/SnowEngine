#version 450 core

out vec4 FragColor;

in vec4 color;
in vec2 vTexCords;

uniform sampler2D depthTexture;

uniform vec2  screenSize;
uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth) {

    float z = depth * 2.0 - 1.0;

    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main() {

    const vec2 uv = gl_FragCoord.xy / screenSize;

    float sceneDepth    = texture(depthTexture, uv).r;
    float particleDepth = gl_FragCoord.z;

    sceneDepth    = linearizeDepth(sceneDepth);
    particleDepth = linearizeDepth(particleDepth);

    float diff = sceneDepth - particleDepth;

    const float softness = 0.8;
    const float fade = clamp(diff / softness, 0.0, 1.0);

    FragColor = vec4(color.rgb, color.a * fade);
}