#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

in vec3 T;
in vec3 B;
in vec3 N;

in mat3 TBN;

uniform sampler2D texture0;
uniform sampler2D texture1;

#include <commonlight.glsl>

void main() {

    vec3 tex    = texture(texture0, vTexCords).rgb;
    vec3 color  = 0.4 * tex; // Ambient
        
    vec3 normal = texture(texture1, vTexCords).xyz;
    normal = (normal * 2.0 - 1.0);

    normal = normalize(TBN * normal);

    normal = normal;

    // Directional Lighting
    // color += calcDirectionalLight(tex, normal);

    // float shadow = calcDirectShadow();
    // color *= (1.0 - shadow);

    // Point Shadow
    for (int i = 0; i < light_count; i++) {

        vec3 lightColor = vec3(0.0);

        lightColor += calcPointLight(pl[i], tex, normal);
        lightColor *= (1.0 - calcShadow(pl[i], depthMap[i]));

        color += lightColor;
    }

    FragColor = vec4(color, 1.0);
    // FragColor = vec4(normal * 0.5 + 0.5, 1.0);
}