#pragma once

#include <glm/glm.hpp>
#include <vector>

#define MAX_PARTICLES 1000u
#define PARTICLE_SPEED 1.0f

using TextureHandle = uint32_t;

struct Particle {

    glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    glm::vec3 startColor;
    glm::vec3 endColor;

    glm::vec2 size;
    glm::vec2 maxSize;

    float shrinking_rate;

    float lifetime;
    float remainingLife;

    bool isActive  = false;
    bool isLooping = false;
};

struct ParticleInitProperties {

    // not suitable for Different Shapes
    // This will spawn particle in AABB box
    glm::vec3 position_min;
    glm::vec3 position_max;

    // SUITABLE
    glm::vec3 velocity_min;
    glm::vec3 velocity_max;

    float size_min;
    float size_max;

    float lifetime_min;
    float lifetime_max;
};

struct ParticleEffect {

    glm::vec3 random_position;
    glm::vec3 random_velocity;
    glm::vec3 random_color;
    glm::vec2 random_size;
    float     random_lifetime;
};

enum random_flags : uint8_t {

    RANDOM_POSITION = 1 << 0,
    RANDOM_VELOCITY = 1 << 1,
    RANDOM_LIFETIME = 1 << 2,
    RANDOM_SIZE     = 1 << 3
};

enum class SpawnShape : uint8_t {

    POINT,
    SPHERE,
    BOX

};

namespace gfx::particles {

    enum Type {
        COLORED,
        TEXTURED
    };

    inline Particle Fire;
    inline Particle Rain;
    inline Particle Smoke;

    inline ParticleInitProperties FireProperties;
    inline ParticleInitProperties RainProperties;
    inline ParticleInitProperties SmokeProperties;
    
    void init();
};

class ParticleEmitter {

    std::vector <Particle> particles;

    TextureHandle particleTexture;
    uint32_t      activeParticles;

public:
    
    bool particleType;

    ParticleEmitter();
    
    void create(const Particle& particle, const glm::vec3& velocity_variation, const uint32_t count);
    void create(const Particle& particle, const uint32_t count, const uint8_t flags = 0);
    void create(const Particle& particle, const ParticleInitProperties& property, const uint32_t count);

    void emit(const glm::vec3& position);

    void update(const float dt);
    // void update(const ParticleEffect& effect, const float dt);

    void render() const;
};