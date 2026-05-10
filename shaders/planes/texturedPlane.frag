#version 450 core
#define MAX_LIGHTS 4

out vec4 FragColor;

in vec3 vPos;
in vec2 vTexCords;

struct pointLight{

    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

uniform sampler2D texture0;

uniform vec3       vNormal;
uniform vec3       camPos;
uniform int        light_count;
uniform pointLight pl[MAX_LIGHTS];

vec3 calcPointLight(pointLight light, vec3 tex);

void main() {

    vec3 tex   = texture(texture0, vTexCords).rgb;
    vec3 color = 0.4 * tex; // Ambient

    for (int i = 0; i < light_count; i++) {
        color += calcPointLight(pl[i], tex);
    }

    FragColor = vec4(color, 1.0);
}

vec3 calcPointLight(pointLight light, vec3 tex) {

    // Diffuse
    vec3  light_dir    = normalize(light.position - vPos);
    float diffuse      = max(dot(vNormal, light_dir), 0.0);
    vec3  diffuseLight = diffuse * tex * light.color;

    // Specular
    vec3 view_dir    = normalize(camPos - vPos);
    vec3 reflect_dir = reflect(-light_dir, vNormal);

    float spec          = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    vec3  specularLight = spec * tex * light.color;

    // Attenuation
    float frag_dist   = length(light.position - vPos);
    float attenuation = 1.0 / (light.constant + light.linear*frag_dist + light.quadratic*frag_dist*frag_dist);

    return (attenuation * vec3(diffuseLight + specularLight));
}