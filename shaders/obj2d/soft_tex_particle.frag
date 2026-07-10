#version 450 core

out vec4 FragColor;
in  vec2 vTexCords;

uniform sampler2D particle;
uniform sampler2D depthTexture;

uniform vec2  screenSize;
uniform float alpha;

uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth) {

    float z = depth * 2.0 - 1.0;

    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main() {
    
    const vec4 tex = texture(particle, vTexCords);

    const float gamma = 2.3;
    vec3 finalColor = pow(tex.rgb, vec3(1.0 / gamma));   // Gamma Correction

    // Soft

    const vec2 uv = gl_FragCoord.xy / screenSize;

    float sceneDepth    = texture(depthTexture, uv).r;
    float particleDepth = gl_FragCoord.z;

    sceneDepth    = linearizeDepth(sceneDepth);
    particleDepth = linearizeDepth(particleDepth);

    float diff = sceneDepth - particleDepth;

    const float softness = 0.8;
    const float fade = clamp(diff / softness, 0.0, 1.0);

    FragColor = vec4(finalColor.rgb, alpha * fade * tex.a);
}