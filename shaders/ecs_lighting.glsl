#define MAX_LIGHTS 4

struct directionalLight {

    vec3 direction;
    vec3 color;
};

struct pointLight{

    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {

    vec3 position;
    vec3 direction;
    vec3 color;

    bool isVisible;

    float cutOffangle;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

// For Phong --------------//

uniform vec3  camPos;
uniform float far_plane;
uniform int   light_count;

uniform pointLight  pl[MAX_LIGHTS];
uniform samplerCube depthMap[MAX_LIGHTS];

uniform directionalLight dl;
uniform sampler2D dl_depthMap;
uniform bool useDirectionalLight;

uniform bool useSpotLight;
uniform SpotLight sl;

uniform float skyboxIntensity;

//-------------------------//

const float pi = 3.14159265359;
float ambientStrength = (0.1 + skyboxIntensity / 2);

vec3 calcDirectionalLight(vec3 tex, vec3 normal) {

    vec3 light_dir = normalize(dl.direction);

    // Diffuse
    float diffuse      = max(dot(normal, light_dir), 0.0);
    vec3  diffuseLight = diffuse * tex * dl.color;

    // Specular
    vec3 view_dir    = normalize(camPos - vPos);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float spec          = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    vec3  specularLight = spec * tex * dl.color;

    return (diffuseLight + specularLight);
}

vec3 calcDirectionalLight_OnlyDiffuse(vec3 tex, vec3 normal) {

    vec3 light_dir = normalize(dl.direction);

    // Diffuse
    float diffuse      = max(dot(normal, light_dir), 0.0);
    vec3  diffuseLight = diffuse * tex * dl.color;

    return diffuseLight;
}

float calcDirectShadow() {

    if (lightSpace_vPos.w <= 0.0) {
        return 0.0;
    }

    vec3 shadowCords = lightSpace_vPos.xyz / lightSpace_vPos.w;
    shadowCords      = shadowCords * 0.5 + 0.5;

    if (shadowCords.x < 0.0 || shadowCords.x > 1.0 ||
        shadowCords.y < 0.0 || shadowCords.y > 1.0 ||
        shadowCords.z > 1.0 || shadowCords.z < 0.0) {
        return 0.0;
    }

    float shadow = 0.0;
    float bias   = 0.25;

    // Filtering
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dl_depthMap, 0);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {

            float PCFdepth = texture(dl_depthMap, shadowCords.xy + vec2(x,y) * texelSize).r;
            shadow += shadowCords.z - bias > PCFdepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;
}

vec3 calcPointLight(pointLight light, vec3 tex, vec3 normal) {

    // Diffuse
    vec3  light_dir    = normalize(light.position - vPos);
    float diffuse      = max(dot(normal, light_dir), 0.0);
    vec3  diffuseLight = diffuse * tex * light.color;

    // diffuseLight = vec3(0.0);

    // Specular
    vec3 view_dir    = normalize(camPos - vPos);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float spec          = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    vec3  specularLight = spec * tex * light.color;

    // Attenuation
    float frag_dist   = length(light.position - vPos);
    float attenuation = 1.0 / (light.constant + light.linear*frag_dist + light.quadratic*frag_dist*frag_dist);

    return (attenuation * (diffuseLight + specularLight));
    // return (diffuseLight + specularLight);
}

vec3 calcPointLight_OnlyDiffuse(pointLight light, vec3 tex, vec3 normal) {

    // Diffuse
    vec3  light_dir    = normalize(light.position - vPos);
    float diffuse      = max(dot(normal, light_dir), 0.0);
    vec3  diffuseLight = diffuse * tex * light.color;

    // Attenuation
    float frag_dist   = length(light.position - vPos);
    float attenuation = 1.0 / (light.constant + light.linear*frag_dist + light.quadratic*frag_dist*frag_dist);

    return (attenuation * (diffuseLight));
}

vec3 calcSpecPointLight(pointLight light, vec3 tex, vec3 tex2, vec3 normal) {

    // Diffuse
    vec3  light_dir    = normalize(light.position - vPos);
    float diffuse      = max(dot(normal, light_dir), 0.0);
    vec3  diffuseLight = diffuse * tex * light.color;

    // diffuseLight = vec3(0.0);

    // Specular
    vec3 view_dir    = normalize(camPos - vPos);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float spec          = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    vec3  specularLight = spec * tex2 * light.color;

    // Attenuation
    float frag_dist   = length(light.position - vPos);
    float attenuation = 1.0 / (light.constant + light.linear*frag_dist + light.quadratic*frag_dist*frag_dist);

    attenuation = 1.0;

    return (attenuation * vec3(diffuseLight + specularLight));
}

float calcShadow(pointLight light, samplerCube map) {

    vec3  fragToLight = vPos - light.position;
    float currentDepth = length(fragToLight);

    float shadow  = 0.0;
    float bias    = 0.05;
    float samples = 4.0;
    float offset  = 0.1;

    for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {

                float closestDepth = texture(map, fragToLight + vec3(x,y,z)).r;
                closestDepth *= far_plane;

                if (currentDepth - bias > closestDepth) {
                    shadow += 1.0;
                }
            }
        }
    }

    shadow /= (samples * samples * samples);

    return shadow;
}

vec3 calcSpotLight(vec3 tex, vec3 normal) {

    vec3 light_dir = normalize(sl.position - vPos);

    float theta = dot(light_dir, normalize(-sl.direction));

    // Diffuse
    float diffuse      = max(dot(normal, light_dir), 0.0);
    vec3  diffuseLight = diffuse * tex * sl.color;

    // Specular
    vec3 view_dir    = normalize(camPos - vPos);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float spec          = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    vec3  specularLight = spec * tex * sl.color;

    // Attenuation
    float frag_dist   = length(sl.position - vPos);
    float attenuation = 1.0 / (sl.constant + sl.linear*frag_dist + sl.quadratic*frag_dist*frag_dist);

    // Smooth-Edge
    float epsilon   = sl.cutOffangle - sl.outerCutOff;
    float intensity = clamp((theta - sl.outerCutOff) / epsilon, 0.0, 1.0);

    return (attenuation * intensity * vec3(diffuseLight + specularLight));
}