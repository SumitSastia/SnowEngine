#version 450 core

#define MAX_PARTICLES 10000u

struct Particle {

    vec4 position;       // 16
    vec4 velocity;       // 16
    vec4 color;          // 16

    float lifetime;      // 4
    float remainingLife; // 4

    float size;          // 4
    uint  isActive;      // 4
};

struct HighParticle {

    vec4 position;       // 16
    vec4 velocity;       // 16
    vec4 acceleration;   // 16

    vec4 startProp;      // 16
    vec4 endProp;        // 16

    vec4 color;          // 16

    float lifetime;      // 4
    float remainingLife; // 4

    float size;          // 4
    uint  isActive;      // 4
};

struct ParticleInitProperties {

    vec4 position;
    vec4 box_size;

    vec4 acc_min;
    vec4 acc_max;

    vec4 velocity_min;
    vec4 velocity_max;

    float size_min;
    float size_max;
    float lifetime_min;
    float lifetime_max;

    float radius;
    float height;
    uint  spawnerType;
    uint  total_count;
};

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCords;

layout (std140, binding = 0) uniform CameraData {

    mat4 projection;
    mat4 view;
    vec3 camPos;
    float padding1;
};

layout (std430, binding = 1) buffer ParticleData {

    uint total_particles;
    uint padding[3];

    ParticleInitProperties properties;

    // Particle particles[MAX_PARTICLES];
    HighParticle particles[MAX_PARTICLES];
};

out vec4 color;

void main(){
    
    int id = gl_InstanceID;

    if (id >= total_particles || particles[id].isActive == 0) {

        gl_Position = vec4(0.0);
        return;
    }

    const vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    const vec3 cameraUp    = vec3(view[0][1], view[1][1], view[2][1]);
    const vec3 cameraFront = vec3(view[0][2], view[1][2], view[2][2]);

    mat4 model;

    model[0] = vec4(cameraRight * particles[id].size, 0.0);
    model[1] = vec4(cameraUp    * particles[id].size, 0.0);
    model[2] = vec4(cameraFront,                      0.0);
    model[3] = vec4(particles[id].position.xyz,       1.0);

    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);

    // Particle p = particles[id];
    HighParticle p = particles[id];
    color = p.color;
}