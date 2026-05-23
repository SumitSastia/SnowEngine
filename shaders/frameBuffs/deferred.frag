#version 450 core

in  vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTexture;

void main() {

    vec3 vPos    = texture(gPosition, vTexCords).xyz;
    vec3 vNormal = texture(gNormal, vTexCords).xyz;
    vec3 albedo  = texture(gTexture, vTexCords).rgb;

    albedo = pow(albedo, vec3(1.0 / 2.3));   // Gamma Correction

    FragColor = vec4(albedo, 1.0);
}