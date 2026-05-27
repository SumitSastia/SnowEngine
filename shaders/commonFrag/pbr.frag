#version 450 core

// out vec4 FragColor;

layout (location = 0) out vec4 FragColor; 
layout (location = 1) out vec4 BrightColor; 

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;
in vec4 lightSpace_vPos;

uniform sampler2D texture0; // albedo
uniform sampler2D texture1; // metallic
uniform sampler2D texture2; // roughness

uniform samplerCube irradianceMap;
uniform samplerCube preFilterMap;
uniform sampler2D brdfLUT;
uniform bool useIrradiance;

uniform samplerCube env;

#include <commonlight.glsl>

void main() {

    vec3 tex = texture(texture0, vTexCords).rgb;

    float metallic  = texture(texture0, vTexCords).r;
    float roughness = texture(texture0, vTexCords).r;

    metallic = 0.8;
    roughness = 0.2;

    // Ambient
    vec3 color = vec3(0.05) * tex;

    // Directional Lighting
    // color += calcDirectionalLight(tex, vNormal);

    // float shadow = calcDirectShadow();
    // color *= (1.0 - shadow);

    // PBR Lighting
    vec3 N = vNormal;
    vec3 V = normalize(camPos - vPos);
    vec3 R = reflect(-V, vNormal);
    vec3 reflected_color = texture(env, R).rgb;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, tex, metallic);

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

    // if (useIrradiance) color = envDiffuse  + metallic * kS * reflected_color;
    if (useIrradiance) color = envDiffuse + kD * specular;

    for (int i = 0; i < light_count; i++) { 

        vec3 Lo = calcPBR(pl[i], tex, vNormal, F0, metallic, roughness);
        Lo *= (1.0 - calcShadow(pl[i], depthMap[i]));;
        
        color += Lo;
	}

    if (useSpotLight) {
        color += calcSpotLight(tex, vNormal);
    }

    // Final Calculation
    FragColor = vec4(color, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) 
        BrightColor = FragColor;
    else 
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}