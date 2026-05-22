#version 450 core

in vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D screen;
uniform sampler2D bloom;

void main() {
	
	const float gamma         = 2.2;
	const float bloomStrength = 0.5;

    vec4 tex = texture(screen, vTexCords);
	vec3 mapped = tex.rgb;

	float emission = tex.a;

    // tone-mapping
	if (emission > 0.0) {

		mapped = mapped / (mapped + vec3(1.0));     // Reinhard Mapping
		mapped = pow(mapped, vec3(1.0 / gamma));   // Gamma Correction
	}

	FragColor = vec4(mapped, 1.0);
	FragColor.rgb += bloomStrength * texture(bloom, vTexCords).rgb;
}