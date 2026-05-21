#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCords;
layout (location = 3) in vec3 aTangent;

out vec3 vPos;
out vec2 vTexCords;
out vec4 lightSpace_vPos;

out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpace;

uniform mat3 normalMatrix;

void main(){

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vPos      = vec3(model * vec4(aPos, 1.0));
    vTexCords = aTexCords;

    lightSpace_vPos = lightSpace * vec4(vPos, 1.0);

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    
    T = normalize(T - dot(T,N) * N);

    vec3 B = -cross(N,T);
    
    TBN = mat3(T,B,N);
}
