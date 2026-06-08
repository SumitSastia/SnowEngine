#version 450 core

in  vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTexture;
uniform sampler2D gOcclusion;

uniform samplerCube irradianceMap;
uniform samplerCube preFilterMap;
uniform sampler2D   brdfLUT;

uniform bool toggleAO;
uniform bool useIrradiance;
uniform bool useDirectionalLight;

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

    float metallic  = albedo.r;
    float roughness = albedo.g;

    // metallic = 0.8;
    // roughness = 0.2;

    vec3 tex = albedo.rgb;
    // tex = vec3(0.95);

    vec3 color = 0.05 * tex;
    vec3 normal = normalize(vNormal);

    if (toggleAO) {

        float occlusion = texture(gOcclusion, vTexCords).r;
        color *= occlusion;
    }

    // Directional Lighting
    if (useDirectionalLight) {

        vec3 dirColor = calcDirectionalLight(vPos, tex, normal);
        // Currently not usable (required: lightSpace_vPos through geometry pass)
        // float shadow  = calcDirectShadow();
        // dirColor *= (1.0 - shadow);
        color += dirColor;
    }

    // PBR Lighting
    vec3 N = vNormal;
    vec3 V = normalize(camPos - vPos);
    vec3 R = reflect(-V, vNormal);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, tex, metallic);

    if (useIrradiance) {

        vec3 kS  = fresnalSchlick(max(dot(vNormal,V), 0.0), F0);
        vec3 kD = (1.0 - kS);
        
        // IBL Diffuse
        vec3 envDiffuse = texture(irradianceMap, vNormal).rgb * tex * kD;
        
        // IBL Specular
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(preFilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;

        vec3 F        = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
        
        // color = envDiffuse  + metallic * kS * reflected_color;
        color = envDiffuse + kD * specular;
        // color = envDiffuse;
    }

    // Point Shadow
    for (int i = 0; i < light_count; i++) {

        // vec3 lightColor = calcPointLight(pl[i], vPos, tex, normal);
        vec3 lightColor = calcPBR(pl[i], vPos, tex, vNormal, F0, metallic, roughness);
        // const float shadow = (1.0 - calcShadow(pl[i], vPos, depthMap[i]));

        // lightColor *= shadow;
        color += lightColor;
    }

    if (useSpotLight) {
        color += calcSpotLight(vPos, tex, normal);
    }

    // color = albedo.rgb;
    color = tone_mapping(color);
    color = pow(color, vec3(1.0 / 2.3));   // Gamma Correction

    FragColor = vec4(color, 1.0);
}