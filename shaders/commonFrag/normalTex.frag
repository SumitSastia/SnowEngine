#version 450 core

// out vec4 FragColor;

layout (location = 0) out vec4 FragColor; 
layout (location = 1) out vec4 BrightColor; 

in vec3 vPos;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

in mat3 TBN;

uniform sampler2D texture0;
uniform sampler2D texture1;

#include <commonlight.glsl>

void main() {

    vec3 tex    = texture(texture0, vTexCords).rgb;
    vec3 normal = texture(texture1, vTexCords).xyz;

    vec3 color = ambientStrength * tex; // Ambient
        
    normal = (normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    // Directional Lighting
    // vec3 dirColor = calcDirectionalLight(tex, normal);
    // float shadow  = calcDirectShadow();

    // dirColor *= (1.0 - shadow);
    // color += dirColor;

    // Point Shadow
    for (int i = 0; i < light_count; i++) {

        vec3 lightColor = calcPointLight(pl[i], tex, normal);
        lightColor *= (1.0 - calcShadow(pl[i], depthMap[i]));

        color += lightColor;
    }

    if (useSpotLight) {
        color += calcSpotLight(tex, normal);
    }

    FragColor = vec4(color, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) 
        BrightColor = FragColor;
    else 
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    // FragColor = vec4(normal * 0.5 + 0.5, 1.0);
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}