#pragma once

#include <glm/glm.hpp>
#include <vector>

#define MAX_PARTICLES 1000

struct Particle {

    glm::vec3 position;
    glm::vec3 velocity;

    glm::vec4 color;

    float size;
    float lifetime;
    float remainingLife;

    bool isActive = false;
};

class ParticleEmitter {

    std::vector <Particle> particles;

public:

    ParticleEmitter(): particles(MAX_PARTICLES) {}

    void create(const Particle& particle);
    void emit();
    void update(const float dt);
    void render() const;
};