#version 450 core

layout (local_size_x = 4) in;

layout (std430, binding = 0) buffer Data {

    float numbers[];
};

void main() {
    
    uint id = gl_GlobalInvocationID.x;
}