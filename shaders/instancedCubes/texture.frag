#version 450 core
#define MAX_LIGHTS 3

struct material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct directionalLight{
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

struct spotLight{
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

//*************************************************************************************//

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

//*************************************************************************************//

uniform samplerCube depthCubeMap[MAX_LIGHTS];
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 viewPos;
uniform float far_plane;

//*************************************************************************************//

uniform sampler2D texture1;
uniform sampler2D texture2;

//*************************************************************************************//

uniform material m1;
uniform spotLight s1;
uniform directionalLight d1;
uniform pointLight p1;

//*************************************************************************************//

uniform samplerCube skybox;
uniform float skyboxIntensity;

uniform sampler2D depthMap;

//*************************************************************************************//

uniform int lights_count;
uniform pointLight plights[MAX_LIGHTS];

//*************************************************************************************//

float init_shadow(vec3 vPos, samplerCube depthCubeMap, vec3 lightPos) {

    vec3 fragToLight = vPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.05;
    float samples = 4.0;
    float offset = 0.1;

    for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {

                float closestDepth = texture(depthCubeMap, fragToLight + vec3(x,y,z)).r;
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

vec3 init_pointLight(pointLight pl, vec3 normal, vec3 vPos, vec3 viewPos, vec3 t1, vec3 t2){

    vec3 lightDirection = normalize(pl.position - vPos);

    // Diffuse
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuseLight = diff * (m1.diffuse*(t1-t2) + t2) * pl.color;

    // Specular
    vec3 viewDirection = normalize(viewPos - vPos);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), m1.shininess);
    vec3 specularLight = spec * t2 * pl.color;

    // Attenuation
    float fragDistance = length(pl.position - vPos);
    float attenuation = 1.0 / (pl.constant + pl.linear*fragDistance + pl.quadratic*fragDistance*fragDistance);

    return (attenuation * vec3(diffuseLight + specularLight));
}

vec3 init_directionalLight(directionalLight dl, vec3 normal, vec3 vPos, vec3 viewPos, vec3 t1, vec3 t2){

    vec3 lightDirection = normalize(dl.direction);

    // Diffuse
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuseLight = diff * (m1.diffuse*(t1-t2) + t2) * dl.color;

    // Specular
    vec3 viewDirection = normalize(viewPos - vPos);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), m1.shininess);
    vec3 specularLight = spec * t2 * dl.color;

    return vec3(diffuseLight + specularLight);
}

vec3 init_spotLight(spotLight sl, vec3 normal, vec3 vPos, vec3 viewPos, vec3 t1, vec3 t2){

    vec3 lightDirection = normalize(sl.position - vPos);

    float theta = dot(lightDirection, normalize(-sl.direction));

    // Diffuse
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuseLight = diff * (m1.diffuse*(t1-t2) + t2) * sl.color;

    // Specular
    vec3 viewDirection = normalize(viewPos - vPos);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), m1.shininess);
    vec3 specularLight = spec * t2 * sl.color;

    // Attenuation
    float fragDistance = length(sl.position - vPos);
    float attenuation = 1.0 / (sl.constant + sl.linear*fragDistance + sl.quadratic*fragDistance*fragDistance);

    // Smooth-Edge
    float epsilon = sl.cutOffangle - sl.outerCutOff;
    float intensity = clamp((theta - sl.outerCutOff) / epsilon, 0.0, 1.0);

    return (attenuation * intensity * vec3(diffuseLight + specularLight));
}

void main(){

    vec3 normal = normalize(vNormal);
    
    vec3 t1 = (texture(texture1, vTexCords)).rgb;
    vec3 t2 = (texture(texture2, vTexCords)).rgb;
    
    float alpha = texture(texture1, vTexCords).a;

    vec3 finalColor = vec3(0.0);
    vec3 lightColors[MAX_LIGHTS];

    // PointLight
    for (int i = 0; i < lights_count; i++) {
        
        lightColors[i] = init_pointLight(plights[i], normal, vPos, viewPos, t1, t2);
        lightColors[i] *= (1.0 - init_shadow(vPos, depthCubeMap[i], lightPos[i]));
    }

    for (int i = 0; i < lights_count; i++) {
        
        finalColor += lightColors[i];
    }

    if(s1.isVisible){
        finalColor += init_spotLight(s1, normal, vPos, viewPos, t1, t2);
    }    

    vec3 incident_ray = normalize(vPos - viewPos);

    // Reflection of Skybox
    vec3 reflected_ray = reflect(incident_ray, normal);
    vec4 reflected_color = vec4(texture(skybox, reflected_ray).rgb, 1.0);

    // Ambient
    vec3 ambientLight = vec3(((m1.ambient * t1) + skyboxIntensity * t1) / 2.00);

    FragColor = vec4(ambientLight + finalColor, alpha);

    // Bloom
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	
	if (brightness > 0.75) {
		BrightColor = FragColor;
	}
	else {
		BrightColor = vec4(0.0,0.0,0.0,1.0);
	}
}