#pragma once

#include <glm/glm.hpp>
#include <vector>

#define MAX_PARTICLES 1000u
#define PARTICLE_SPEED 1.0f

using TextureHandle = uint32_t;

struct Particle {

    glm::vec3 position;
    glm::vec3 velocity;

    glm::vec4 color;
    glm::vec3 startColor;
    glm::vec3 endColor;

    float size;
    float shrinking_rate;

    float lifetime;
    float remainingLife;

    bool isActive = false;
};

class ParticleEmitter {

    std::vector <Particle> particles;
    TextureHandle particleTexture;

public:

    ParticleEmitter();

    void create();
    void emit(const glm::vec3& position);
    void update(const float dt);
    void render() const;
};