#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCords;

out vec3 vPos;
out vec2 vTexCords;

out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

void main(){

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vPos      = vec3(model * vec4(aPos, 1.0));
    vTexCords = aTexCords;

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    
    T = normalize(T - dot(T,N) * N);

    vec3 B = cross(N,T);
    
    TBN = mat3(T,B,N);
}