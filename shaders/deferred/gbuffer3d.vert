#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCords;

layout (std140, binding = 0) uniform CameraData {

    mat4 projection;
    mat4 view;
    vec3 cameraPos;
};

out vec3 vPos;
out vec3 vNormal;
out vec2 vTexCords;

uniform mat4 model;
uniform mat3 normalMatrix;

void main(){

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vPos      = vec3(model * vec4(aPos, 1.0));
    vNormal   = normalize(normalMatrix * aNormal);
    vTexCords = aTexCords;
}