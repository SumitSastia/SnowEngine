#pragma once

#include <glm/glm.hpp>
#include <vector>

#define MAX_PARTICLES 10000u
#define PARTICLE_SPEED 1.0f

#include <core/config.h>

struct Particle {

    glm::vec3 color = { 1.0f, 1.0f, 1.0f };
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    glm::vec3 startColor;
    glm::vec3 endColor;

    glm::vec2 size;
    glm::vec2 startSize = { 1.00f, 1.00f };
    glm::vec2 endSize   = { 0.01f, 0.01f };

    float alpha = 1.0f;
    float shrinking_rate; // slope, currently linear

    float lifetime;
    float remainingLife;

    float rotation_angle   = 0.0f;
    float angular_velocity = 0.0f;

    bool isActive  = false;
    bool isLooping = false;
};

struct ParticleInitProperties {

    // POINT
    glm::vec3 center;
    
    // BOX
    glm::vec3 box_size;

    glm::vec3 acc_min;
    glm::vec3 acc_max;

    glm::vec3 velocity_min;
    glm::vec3 velocity_max;

    float size_min;
    float size_max;

    float lifetime_min;
    float lifetime_max;

    // SPHERE
    float radius;

    // CONE
    uint32_t height;
    
    uint32_t total_count;
    uint8_t  spawnerType;
};

struct ParticleEffect {

    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 color;

    float size;
    float random_lifetime;
};

enum random_flags : uint8_t {

    RANDOM_POSITION = 1 << 0,
    RANDOM_VELOCITY = 1 << 1,
    RANDOM_LIFETIME = 1 << 2,
    RANDOM_SIZE     = 1 << 3
};

namespace gfx::particles {

    enum PType : uint8_t {
        COLORED,
        TEXTURED
    };

    enum SpawnShape : uint8_t {
        POINT,
        SPHERE,
        BOX,
        CONE
    };

    struct SpawnProperties {
        
        // POINT
        glm::vec3 center;
        
        // BOX
        glm::vec3 box_size;
        
        // SPHERE
        float radius;

        // CONE
        uint32_t height;
        
        uint32_t total_count;
        uint8_t  spawnerType;

        SpawnProperties():
            center({0.0f}),
            box_size({1.0f}),
            radius(1.0f),
            total_count(1u),
            spawnerType(0u) {
        }

    };

    inline Particle Fire;
    inline Particle Rain;
    inline Particle smoke;
    inline Particle flash;

    inline ParticleInitProperties FireProperties;
    inline ParticleInitProperties RainProperties;
    inline ParticleInitProperties smokeProperties;
    inline ParticleInitProperties flashProperties;
    
    void init();
};

class ParticleEmitter {

    std::vector <Particle> particles;
    ParticleInitProperties properties;

    TextureHandle particleTexture;
    uint32_t      activeParticles; // not implemented

    const glm::vec3 generate(const ParticleInitProperties& spawner);

public:
    
    float softness = 0.0f;
    bool  particleType;

    ParticleEmitter();

    void create(const Particle& particle, const uint32_t count, const uint8_t flags = 0);
    void create(const Particle& particle, const ParticleInitProperties& property);
    void recreate(Particle& particle);

    void set(const ParticleInitProperties& properties) { this->properties = properties; }
    void setTexture(const TextureHandle& handle) { particleTexture = handle; }

    void emit(const glm::vec3& position);

    void update(const float dt);
    // void update(const ParticleEffect& effect, const float dt);

    void render(const TextureHandle depthTexture = 0) const;
};

class InstancedParticles {

public:
    void init();

private:
    uint VAO, VBO1, VBO2, VBO3;
};