#version 450 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gTexture;

in vec3 vPos;
in vec2 vTexCords;

in vec3 tangentFragPos;
in vec3 tangentViewPos;

in mat3 TBN;

uniform sampler2D texture0; // Diffuse
uniform sampler2D texture1; // Normal
uniform sampler2D texture2; // Parallax

uniform float height_scale;

/*
!!!!!!!!!!!!!!!! WORK IN PROGRESS !!!!!!!!!!!!!!!!
*/

void main() {

	gPosition = vPos;
	gTexture  = texture(texture0, vTexCords);

    vec3 normal = texture(texture1, vTexCords).xyz;
    
    normal = (normal * 2.0 - 1.0);
    gNormal = normalize(TBN * normal);

	// gTexture = vec4(vec3(0.9), 1.0);
}