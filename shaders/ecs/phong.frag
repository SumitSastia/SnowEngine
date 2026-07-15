#version 450 core

// out vec4 FragColor;

layout (location = 0) out vec4 FragColor; 
layout (location = 1) out vec4 BrightColor; 

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

uniform sampler2D albedo;

#include <ecs_lighting.glsl>

void main() {

    vec3 tex   = texture(albedo, vTexCords).rgb;
    vec3 color = 0.01 * tex; // Ambient

    // if (useDirectionalLight) {

        vec3 dirLightColor = calcDirectionalLight_OnlyDiffuse(tex, vNormal);

        float shadow   = calcDirectShadow();
        dirLightColor *= (1.0 - shadow);

        color += dirLightColor;
    // }

    // Point Shadow
    for (int i = 0; i < lightCount; i++) {

        vec3 lightColor = calcPointLightDiffuse(lights[i], tex, vNormal);
        lightColor *= (1.0 - calcShadowUBO(lights[i].position, depthMap[i]));

        color += lightColor;
    }

    // if (useSpotLight) {
    //     color += calcSpotLight(tex, vNormal);
    // }

    FragColor = vec4(color, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) 
        BrightColor = FragColor;
    else 
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    // FragColor = vec4(vNormal * 0.5 + 0.5, 1.0);
}