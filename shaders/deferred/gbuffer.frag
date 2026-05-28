#version 450 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gTexture;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCords;

uniform sampler2D texture0;

void main() {

	gPosition = vPos;
	gNormal   = normalize(vNormal);
	gTexture  = texture(texture0, vTexCords);

	gTexture = vec4(vec3(0.95), 1.0);
}