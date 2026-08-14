#version 450 core

// layout (local_size_x = 128) in;
layout (local_size_x = 16, local_size_y = 16) in;

layout (std430, binding = 0) buffer Data {

    uint dataSize;
    float numbers[];
};

layout (rgba32f, binding = 0) uniform image2D outputImage;

uniform vec2 resolution;
uniform float time;

void main() {
    
    // uint id = gl_GlobalInvocationID.x;
    
    // if (id >= dataSize) return;

    // numbers[id] += (float(id) / float(dataSize)) + 0.0001;
    // // numbers[id] = float(id % 32) / 31.0;
    // if (numbers[id] > 1.0) numbers[id] = 0.0;

    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

    if (pixel.x >= int(resolution.x) ||
        pixel.y >= int(resolution.y))
        return;

    vec2 uv = vec2(pixel) / resolution;

    // 2. Animated Gradient

    float value = sin(uv.x * 10.0 + time);
    value = value * 0.5 + 0.5;

    // imageStore(
    //     outputImage,
    //     pixel,
    //     // vec4(uv.x, uv.y, 0.0, 1.0)
    //     vec4(value, 0.0, 1.0 - value, 1.0)
    // );

    // 3. Plasma Effect

    float v = 0.0;
    
    v += sin(uv.x * 10.0 + time);
    v += sin(uv.y * 10.0 + time);
    v += sin((uv.x + uv.y) * 10.0 + time);
    v += sin(length(uv - 0.5) * 20.0 - time);

    v = v * 0.25 + 0.5;

    vec3 color = vec3(
        sin(v * 6.28) * 0.5 + 0.5,
        sin(v * 6.28 + 2.0) * 0.5 + 0.5,
        sin(v * 6.28 + 4.0) * 0.5 + 0.5
    );

    imageStore(
        outputImage,
        pixel,
        vec4(color, 1.0)
    );
}