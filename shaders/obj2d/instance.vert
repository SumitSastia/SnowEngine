#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCords;
layout (location = 2) in vec4 instanceColor;
layout (location = 3) in vec4 instancePos;
layout (location = 4) in vec2 instanceSize;

layout (std140, binding = 0) uniform CameraData {

    mat4 projection;
    mat4 view;
    vec3 camPos;
    float padding1;
};

out vec4 color;
out vec2 vTexCords;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main(){
    
    float c = cos(radians(instancePos.w));
    float s = sin(radians(instancePos.w));

    vec2 local = {
        aPos.x * c - aPos.y * s,
        aPos.x * s + aPos.y * c
    };

    local *= instanceSize;

    const vec3 worldPos = instancePos.xyz + (cameraRight * local.x + cameraUp * local.y);
    gl_Position = projection * view * vec4(worldPos, 1.0);

    color     = instanceColor;
    vTexCords = aTexCords;
}