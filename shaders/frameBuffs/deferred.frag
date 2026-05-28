#version 450 core

in  vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTexture;
uniform sampler2D gOcclusion;

#include <deferred_lighting.glsl>

vec3 tone_mapping(vec3 color) {

    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float mappedLum = luminance / (luminance + 1.0);

    // Scale color by ratio of mapped/original luminance
    return color * (mappedLum / luminance);
}

void main() {

    vec3 vPos    = texture(gPosition, vTexCords).xyz;
    vec3 vNormal = texture(gNormal, vTexCords).xyz;
    vec4 albedo  = texture(gTexture, vTexCords);

    float occlusion = texture(gOcclusion, vTexCords).r;

    vec3 tex = albedo.rgb;

    vec3 color = tex * occlusion;
    vec3 normal = normalize(vNormal);

    // Directional Lighting
    // vec3 dirColor = calcDirectionalLight(vPos, tex, normal);
    // float shadow  = calcDirectShadow();
    // dirColor *= (1.0 - shadow);
    // color += dirColor;

    // Point Shadow
    // for (int i = 0; i < light_count; i++) {

    //     vec3 lightColor = calcPointLight(pl[i], vPos, tex, normal);
    //     const float shadow = (1.0 - calcShadow(pl[i], vPos, depthMap[i]));

    //     lightColor *= shadow;
    //     color += lightColor;
    // }

    if (useSpotLight) {
        color += calcSpotLight(vPos, tex, normal);
    }

    // color = albedo.rgb;
    color = tone_mapping(color);
    color = pow(color, vec3(1.0 / 2.3));   // Gamma Correction

    FragColor = vec4(color, 1.0);
}