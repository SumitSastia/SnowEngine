#version 450 core

layout (local_size_x = 4) in;

layout (std430, binding = 0) buffer Data {

    uint dataSize;
    float numbers[];
};

void main() {
    
    uint id = gl_GlobalInvocationID.x;
    
    if (id > dataSize) return;

    numbers[id] += 0.01;
    if (numbers[id] > 1.0) numbers[id] = 0.0;
}