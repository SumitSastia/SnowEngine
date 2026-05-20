#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 vTextureDir;

uniform mat4 projection;
uniform mat4 view;

void main() {
	
	vec4 position = projection * view * vec4(aPos, 1.0);

	vTextureDir = aPos;
	gl_Position = position.xyww;
}