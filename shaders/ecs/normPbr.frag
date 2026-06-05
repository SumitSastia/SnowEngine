#version 450 core

// out vec4 FragColor;

layout (location = 0) out vec4 FragColor; 
layout (location = 1) out vec4 BrightColor; 

in vec3 vPos;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

in mat3 TBN;

uniform sampler2D albedo; 
uniform sampler2D normalMap;  
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

// uniform samplerCube irradianceMap;
// uniform bool useIrradiance;

#include <ecs_lighting.glsl>
#include <ecs_pbr.glsl>

void main() {

    vec3 tex    = texture(albedo, vTexCords).rgb;
    vec3 normal = texture(normalMap, vTexCords).xyz;

    float metallic  = texture(metallicMap, vTexCords).r;
    float roughness = texture(roughnessMap, vTexCords).r;

    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    // Ambient
    vec3 color = vec3(0.05) * tex;

    // Directional Lighting
    if (useDirectionalLight) {

        vec3 dirLightColor = calcDirectionalLight(tex, normal);

        float shadow   = calcDirectShadow();
        dirLightColor *= (1.0 - shadow);

        color += dirLightColor;
    }

    // PBR Lighting
    vec3 V = normalize(camPos - vPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, tex, metallic);

    // vec3 kS  = fresnalSchlick(max(dot(normal,V), 0.0), F0);
    // vec3 kD = (1.0 - kS) * (1.0 - metallic);
    // vec3 kD = (1.0 - kS);

    // vec3 envDiffuse = texture(irradianceMap, normal).rgb * tex * kD;
    // if (useIrradiance) color = envDiffuse;

    for (int i = 0; i < light_count; i++) { 

        vec3 Lo = calcPBR(pl[i], tex, normal, F0, metallic, roughness);
        Lo *= (1.0 - calcShadow(pl[i], depthMap[i]));
        
        color += Lo;
	}

    if (useSpotLight) {
        color += calcSpotLight(tex, normal);
    }

    // Final Calculation
    FragColor = vec4(color, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) 
        BrightColor = FragColor;
    else 
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    
    // FragColor = vec4(normal * 0.5 + 0.5, 1.0);
}