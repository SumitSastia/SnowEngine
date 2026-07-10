#version 450 core

in vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D depthTexture;

void main() {

	float depth = texture(depthTexture, vTexCords).r;

	depth = depth * depth * depth;
	FragColor = vec4(vec3(1.0 - depth), 1.0);
}