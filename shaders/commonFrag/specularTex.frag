#version 450 core

// out vec4 FragColor;

layout (location = 0) out vec4 FragColor; 
layout (location = 1) out vec4 BrightColor; 

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

uniform sampler2D texture0;
uniform sampler2D texture2;

#include <commonlight.glsl>

void main() {

    vec3 tex   = texture(texture0, vTexCords).rgb;
    vec3 tex2  = texture(texture2, vTexCords).rgb;
    vec3 color = ambientStrength * tex; // Ambient

    // Directional Lighting
    // color += calcDirectionalLight(tex, vNormal);

    // float shadow = calcDirectShadow();
    // color *= (1.0 - shadow);

    // Point Shadow
    for (int i = 0; i < light_count; i++) {

        vec3 lightColor = vec3(0.0);

        lightColor += calcSpecPointLight(pl[i], tex, tex2, vNormal);
        lightColor *= (1.0 - calcShadow(pl[i], depthMap[i]));

        color += lightColor;
    }

    if (useSpotLight) {
        color += calcSpotLight(tex, vNormal);
    }

    FragColor = vec4(color, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) 
        BrightColor = FragColor;
    else 
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}