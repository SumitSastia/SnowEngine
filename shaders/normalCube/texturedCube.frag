#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

uniform sampler2D texture0;

#include <commonlight.glsl>

void main() {

    vec3 tex   = texture(texture0, vTexCords).rgb;
    vec3 color = 0.4 * tex; // Ambient

    // Directional Lighting
    // color += calcDirectionalLight(tex, vNormal);

    // float shadow = calcDirectShadow();
    // color *= (1.0 - shadow);

    // Point Shadow
    for (int i = 0; i < light_count; i++) {

        vec3 lightColor = vec3(0.0);

        lightColor += calcPointLight(pl[i], tex, vNormal);
        lightColor *= (1.0 - calcShadow(pl[i], depthMap[i]));

        color += lightColor;
    }

    FragColor = vec4(color, 1.0);
}