#version 450 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 lightColor;

void main(){

    const float gamma = 2.3;

    vec3 color = pow(lightColor, vec3(1.0 / gamma));   // Gamma Correction

    // color = lightColor;

    FragColor   = vec4(color, 0.0);
    BrightColor = FragColor;
    // FragColor   = vec4(0.0, 1.0, 0.0, 1.0);
}