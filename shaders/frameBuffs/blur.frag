#version 450 core

in vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D screen;

uniform bool horizontal;
uniform float weight[5] = float [] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {

	// Gaussian Blur
	vec2 tex_offset = 1.0 / textureSize(screen, 0);
	vec3 result = texture(screen, vTexCords).rgb * weight[0];

	if (horizontal) {
		for (int i = 1; i < 5; i++) {
			
			result += texture(screen, vTexCords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(screen, vTexCords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else {
		for (int i = 1; i < 5; i++) {
			
			result += texture(screen, vTexCords + vec2(tex_offset.y * i, 0.0)).rgb * weight[i];
			result += texture(screen, vTexCords - vec2(tex_offset.y * i, 0.0)).rgb * weight[i];
		}
	}

	FragColor = vec4(result, 1.0);
}