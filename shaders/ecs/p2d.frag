#version 450 core

out vec4 FragColor;

in vec3 vPos;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

in vec3 tangentFragPos;
in vec3 tangentViewPos;

in mat3 TBN;

uniform sampler2D albedo;    // Diffuse
uniform sampler2D normalMap; // Normal
uniform sampler2D heightMap; // Parallax

uniform float height_scale;

#include <ecs_lighting.glsl>

vec2 parallax_mapping(vec3 view_dir) {
    
    // float height = texture(heightMap, vTexCords).r;
    // vec2 p = view_dir.xy / view_dir.z * (height * height_scale);
    
    // return vTexCords - p;

    const float numLayers = 10;

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    vec2 P = view_dir.xy * height_scale;
    vec2 deltaTexCords = P / numLayers;

    vec2  currentTexCords   = vTexCords;
    float currentDepthValue = texture(heightMap, vTexCords).r;

    while (currentLayerDepth < currentDepthValue) {

        currentTexCords   -= deltaTexCords;
        currentDepthValue  = texture(heightMap, currentTexCords).r;
        currentLayerDepth += layerDepth;
    }

    return currentTexCords;

    vec2 prevTexCords = currentTexCords + deltaTexCords;

    float afterDepth  = currentDepthValue - currentLayerDepth;
    float beforeDepth = texture(heightMap, prevTexCords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCords = prevTexCords * weight + currentTexCords * (1.0 - weight);

    return finalTexCords;
}

void main() {

    vec3 view_dir   = normalize(tangentViewPos - tangentFragPos);
    vec2 vTexCords2 = parallax_mapping(view_dir);

    if (vTexCords2.x > 1.0 || vTexCords2.y > 1.0 || vTexCords2.x < 0.0 || vTexCords2.y < 0.0)
        discard;

    vec3 tex    = texture(albedo, vTexCords2).rgb;
    vec3 normal = texture(normalMap, vTexCords2).xyz;

    vec3 color = 0.01 * tex; // Ambient
        
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
        // lightColor *= (1.0 - calcShadow(pl[i], depthMap[i]));

        color += lightColor;
    }

    if (useSpotLight) {
        color += calcSpotLight(tex, normal);
    }

    FragColor = vec4(color, 1.0);
    // FragColor = vec4(normal * 0.5 + 0.5, 1.0);
    // FragColor = vec4(texture(texture2, vTexCords).rgb, 1.0);
}