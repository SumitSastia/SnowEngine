#pragma once

#include <glm/glm.hpp>
#include <vector>

#define MAX_PARTICLES 1000u
#define PARTICLE_SPEED 1.0f

#include <core/config.h>

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

    // POINT
    glm::vec3 center;
    
    // BOX
    glm::vec3 box_size;

    // not suitable for Different Shapes
    // This will spawn particle in AABB box
    // glm::vec3 position_min;
    // glm::vec3 position_max;

    // SUITABLE
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

namespace gfx::particles {

    enum Type {
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
    inline Particle Smoke;

    inline ParticleInitProperties FireProperties;
    inline ParticleInitProperties RainProperties;
    inline ParticleInitProperties SmokeProperties;
    
    void init();
};

class ParticleEmitter {

    std::vector <Particle> particles;
    ParticleInitProperties properties;

    TextureHandle particleTexture;
    uint32_t      activeParticles;

    // void generate(const float radius);                         // SPHERE
    // void generate(const glm::vec3& min, const glm::vec3& max); // BOX

    const glm::vec3 generate(const ParticleInitProperties& spawner);

public:
    
    bool particleType;

    ParticleEmitter();

    void create(const Particle& particle, const uint32_t count, const uint8_t flags = 0);
    void create(const Particle& particle, const ParticleInitProperties& property);
    void recreate(Particle& particle);

    void set(const ParticleInitProperties& properties) { this->properties = properties; }

    void emit(const glm::vec3& position);

    void update(const float dt);
    // void update(const ParticleEffect& effect, const float dt);

    void render() const;
};