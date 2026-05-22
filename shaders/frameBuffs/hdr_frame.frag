#version 450 core

in vec2 vTexCords;
out vec4 FragColor;

uniform sampler2D screen;
uniform bool toggle;

void main() {

    const float gamma    = 2.2;
    const float exposure = 1.0;

    vec3 tex = texture(screen, vTexCords).rgb;

    // tone-mapping
    vec3 mapped = tex;


	if (toggle) 
        mapped = tex / (tex + vec3(1.0));          // Reinhard Mapping
        // mapped = vec3(1.0) - exp(-tex * exposure); // Tone Mapping

    mapped = pow(mapped, vec3(1.0 / gamma));   // Gamma Correction
    
    FragColor = vec4(mapped, 1.0);
}