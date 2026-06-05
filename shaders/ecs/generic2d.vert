#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCords;

out vec3 vPos;
out vec3 vNormal;
out vec2 vTexCords;
out vec4 lightSpace_vPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

void main(){

    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);

    vPos      = vec3(model * vec4(aPos, 0.0, 1.0));
    vNormal   = normalize(normalMatrix * vec3(0.0, 0.0, 1.0));
    vTexCords = aTexCords;

    lightSpace_vPos = lightSpaceMatrix * vec4(vPos, 1.0);
}
