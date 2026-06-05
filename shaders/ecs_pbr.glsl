float distributionGGX(vec3 N, vec3 H, float roughness) {

	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N,H), 0.0);
	float NdotH2 =  NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = pi * denom * denom;

	return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
	
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {

	float NdotV = max(dot(N,V), 0.0);
	float NdotL = max(dot(N,L), 0.0);

	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnalSchlick(float cosTheta, vec3 F0) {

	return F0 +	(1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Global Variables
// 1. vPos, 2. camPos
vec3 calcPBR(pointLight light, vec3 tex, vec3 normal, vec3 F0, float metallic, float roughness) {

    vec3 Lo = vec3(0.0);

    vec3 N  = normal;
    vec3 V  = normalize(camPos - vPos);

    // calculate per-light radiance
    vec3 L = normalize(light.position - vPos);
    vec3 H = normalize(V + L);

    float distance0   = length(light.position - vPos);
    float attenuation = 1.0 / (distance0 * distance0);
    vec3  radiance    = 10.0 * light.color * attenuation;

    // cook-torrance BRDF
    float NDF  = distributionGGX(N, H, roughness);
    float G    = geometrySmith(N, V, L, roughness);
    vec3  F    = fresnalSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(N,V), 0.0) * max(dot(N,L), 0.0) + 0.0001;
    vec3  specular    = numerator / denominator;
    
    // add specular to outgoing radiance
    float NdotL = max(dot(N,L), 0.0);
    Lo += (kD * tex / pi + specular) * radiance * NdotL;

    return Lo;
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {

    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}