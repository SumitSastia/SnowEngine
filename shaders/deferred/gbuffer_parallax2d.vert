#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCords;

layout (std140, binding = 0) uniform CameraData {

    mat4 projection;
    mat4 view;
    vec3 cameraPos;
};

out vec3 vPos;
out vec2 vTexCords;

out mat3 TBN;

out vec3 tangentFragPos;
out vec3 tangentViewPos;

uniform mat4 model;
uniform mat3 normalMatrix;

uniform vec3 viewPos;

void main(){

    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);

    vPos      = vec3(model * vec4(aPos, 0.0, 1.0));
    vTexCords = aTexCords;

    vec3 N = normalize(normalMatrix * vec3(0.0, 0.0, 1.0));
    vec3 T = normalize(normalMatrix * vec3(1.0, 0.0, 0.0));
    
    T = normalize(T - dot(T,N) * N);

    vec3 B = cross(N,T);
    
    TBN = mat3(T,B,N);

    tangentFragPos = TBN * vPos;
    tangentViewPos = TBN * viewPos;
}
