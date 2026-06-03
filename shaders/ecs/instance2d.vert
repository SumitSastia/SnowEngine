#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCords;
layout (location = 2) in mat4 instanceModel;
layout (location = 6) in mat3 instanceNormal;

out vec3 vPos;
out vec3 vNormal;
out vec2 vTexCords;

uniform mat4 projection;
uniform mat4 view;

void main(){

    gl_Position = projection * view * instanceModel * vec4(aPos, 0.0, 1.0);

    vPos      = vec3(instanceModel * vec4(aPos, 0.0, 1.0));
    vNormal   = normalize(instanceNormal * vec3(0.0, 0.0, 1.0));
    vTexCords = aTexCords;
}
