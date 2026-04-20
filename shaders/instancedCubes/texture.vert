#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCords;
layout (location = 3) in mat4 instanceModel;
layout (location = 7) in mat3 instanceNormal;

out vec3 vPos;
out vec3 vNormal;
out vec2 vTexCords;

uniform mat4 projection;
uniform mat4 view;

void main(){

    gl_Position = projection * view * instanceModel * vec4(aPos , 1.0);

    vPos = vec3(instanceModel * vec4(aPos , 1.0));
    vNormal = instanceNormal * aNormal;
    vTexCords = aTexCords;
}
