#version 450 core

in  vec3 vTextureDir;
out vec4 FragColor;

uniform samplerCube cubeMap;

void main() {
	FragColor = vec4(texture(cubeMap, vTextureDir).rgb, 0.0);
}