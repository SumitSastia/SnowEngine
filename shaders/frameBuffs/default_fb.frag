#version 450 core

in vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D screen;

void main() {
	FragColor = vec4(vec3(texture(screen, vTexCords).rgb), 1.0);
}