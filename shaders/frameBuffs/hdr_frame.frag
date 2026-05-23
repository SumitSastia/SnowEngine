#version 450 core

in vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D screen;
uniform bool toggle;
uniform float gamma;

vec3 tone_mapping(vec3 color) {

    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float mappedLum = luminance / (luminance + 1.0);

    // Scale color by ratio of mapped/original luminance
    return color * (mappedLum / luminance);
}

void main() {

    // const float gamma    = 2.2;
    // const float exposure = 1.0;

    vec4 tex = texture(screen, vTexCords);
	vec3 mapped = tex.rgb;

	float emission = tex.a;

    // tone-mapping
    mapped = tone_mapping(mapped);

    if (emission > 0.0 && toggle){

        mapped = tex.rgb;
        mapped = mapped / (mapped + vec3(1.0));          // Reinhard Mapping
        // mapped = vec3(1.0) - exp(-mapped * exposure);    // Tone Mapping

    }
    mapped = pow(mapped, vec3(1.0 / gamma));   // Gamma Correction
    
    FragColor = vec4(mapped, 1.0);
}