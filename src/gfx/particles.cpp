#include <gfx/particles.h>
#include <ecs/entity.h>
#include <math/random.h>
#include <assetManager.h>

#include <shader.h>
#include <camera.h>
#include <input.h>

#include <core/config.h>

namespace gfx::particles {
    
    void init() {

        Fire.acceleration = glm::vec3(
            Random::Float(0.1f),
            0.2f,
            Random::Float(0.1f)
        );

        Fire.velocity = glm::vec3(
            0.0f, Random::Float(0.5f, 0.8f), 0.0f
        );

        Fire.startColor = colors::ORANGE;
        Fire.endColor   = colors::YELLOW;

        // Fire.startColor = colors::BLUE;
        // Fire.endColor   = colors::LIGHT_BLUE;

        Fire.size    = glm::vec2(0.1f);
        Fire.maxSize = Fire.size;

        Fire.lifetime = 1.2f;
        Fire.remainingLife = Fire.lifetime;
        
        Fire.rotation_angle   = 0.0f;
        Fire.angular_velocity = 20.0f;

        FireProperties.spawnerType = gfx::particles::BOX;
        // FireProperties.spawnerType = gfx::particles::SPHERE;

        FireProperties.center   = { 0.0f, 0.0f, 0.0f };
        FireProperties.box_size = { 0.2f, 0.2f, 0.2f };
        // FireProperties.radius   = 1.0f;

        FireProperties.velocity_min = {-0.1f, 0.5f,-0.1f };
        FireProperties.velocity_max = { 0.1f, 0.8f, 0.1f };

        FireProperties.size_min = 1.0f;
        FireProperties.size_max = 1.5f;

        FireProperties.lifetime_min = 1.0f;
        FireProperties.lifetime_max = 2.0f;

        FireProperties.acc_min = glm::vec3(-0.1f, 0.1f,-0.1f);
        FireProperties.acc_max = glm::vec3( 0.1f, 0.4f, 0.1f);

        //-------------------------------------------------------------------//

        Rain.acceleration = glm::vec3(
            -2.0f,
            -1.0f,
             0.0f
        );

        Rain.velocity = glm::vec3(
            -30.0f,
            -20.0f,
             0.0f
        );

        Rain.startColor = colors::BLUE;
        Rain.endColor   = colors::LIGHT_BLUE;

        Rain.size     = glm::vec2(0.1f, 0.2f);
        Rain.maxSize  = Rain.size;
        
        Rain.lifetime = 3.5f;
        Rain.remainingLife = Rain.lifetime;

        // RainProperties.position_min = {-10.0f,-4.0f,-10.0f };
        // RainProperties.position_max = { 10.0f, 4.0f, 10.0f };

        RainProperties.spawnerType = gfx::particles::BOX;

        RainProperties.center   = { 0.0f, 0.0f, 0.0f };
        RainProperties.box_size = { 20.0f, 8.0f, 20.0f };

        RainProperties.velocity_min = {-5.0f,-2.0f, 0.0f };
        RainProperties.velocity_max = {-5.0f,-2.0f, 0.0f };

        RainProperties.size_min = 1.0f;
        RainProperties.size_max = 1.2f;

        RainProperties.lifetime_min = 3.0f;
        RainProperties.lifetime_max = 3.0f;

        RainProperties.acc_min = Rain.acceleration;
        RainProperties.acc_max = Rain.acceleration;

        //-------------------------------------------------------------------//

        flash.velocity     = { 0.0f, 0.0f,-5.0f };
        flash.acceleration = { 0.0f, 0.0f,-5.0f };

        flash.startColor = { 0.976f, 0.811f, 0.341f };
        flash.endColor   = colors::BLACK;

        flash.size    = { 0.05f, 0.05f };
        flash.maxSize = flash.size;

        flash.lifetime = 0.1f;
        flash.remainingLife = flash.lifetime;

        flashProperties.spawnerType = gfx::particles::BOX;
        
        flashProperties.center = { 0.0f, 0.0f, 0.0f };
        flashProperties.box_size = { 0.1f, 0.1f, 0.1f};

        flashProperties.velocity_min = flash.velocity;
        flashProperties.velocity_max = flash.velocity;

        flashProperties.size_min = 1.0f;
        flashProperties.size_max = 1.5f;

        flashProperties.lifetime_min = 1.0f * flash.lifetime;
        flashProperties.lifetime_max = 1.5f * flash.lifetime;

        flashProperties.acc_min = flash.acceleration;
        flashProperties.acc_max = flash.acceleration;
    }
};

ParticleEmitter::ParticleEmitter():
    particles(MAX_PARTICLES),
    particleTexture(0),
    activeParticles(0),
    particleType(gfx::particles::COLORED)
{
    particleTexture = AssetManager::loadTexture("assets/particles/scorch_01.png", true);
}

const glm::vec3 ParticleEmitter::generate(const ParticleInitProperties& spawner) {

    switch (spawner.spawnerType) {

        case gfx::particles::POINT:
            return spawner.center;
        
        case gfx::particles::SPHERE: {

            const glm::vec3 direction = Random::vec3(glm::vec3(-1.0f), glm::vec3(1.0f));
            return (spawner.center + glm::normalize(direction) * Random::Float(0, spawner.radius));
        }

        case gfx::particles::BOX: {

            const glm::vec3 pos_min = -spawner.box_size / 2.0f;
            const glm::vec3 pos_max =  spawner.box_size / 2.0f;

            return (spawner.center + Random::vec3(pos_min, pos_max));
        }

        case gfx::particles::CONE: {

            const float height    = Random::Float(0.0f, spawner.height);
            const float maxRadius = (height / spawner.height) * spawner.radius;

            // for inverted cone: maxRadius = (1.0f - height/spawner.height) * spawner.radius;

            const float angle  = Random::Float(0.0f, glm::two_pi<float>());
            const float radius = maxRadius * glm::sqrt(Random::Float(0.0f, 1.0f));

            return glm::vec3(glm::cos(angle) * radius, height, glm::sin(angle) * radius);
        }

        default:
            return glm::vec3(0.0f);
    }
}

void ParticleEmitter::create(const Particle& particle, const uint32_t count, const uint8_t flags) {
    
    uint32_t amount = 0;

    for (Particle& p : particles) {

        if (!p.isActive) {
            
            p = particle;
            p.isActive = true;

            if (flags & RANDOM_POSITION) {
                p.position.x += Random::Float(-0.5f, 0.5f) * p.position.x;
                p.position.y += Random::Float(-0.5f, 0.5f) * p.position.y;
                p.position.z += Random::Float(-0.5f, 0.5f) * p.position.z;
            }

            if (flags & RANDOM_VELOCITY) {
                p.velocity.x += Random::Float(-0.5f, 0.5f) * p.velocity.x;
                p.velocity.y += Random::Float(-0.5f, 0.5f) * p.velocity.y;
                p.velocity.z += Random::Float(-0.5f, 0.5f) * p.velocity.z;
            }

            if (flags & RANDOM_LIFETIME) {
                p.lifetime += Random::Float(-0.1f, 0.1f) * p.lifetime;
            }
            
            if (flags & RANDOM_SIZE) {
                p.size.x += Random::Float(0.1f, 0.2f) * p.size.x;
                p.size.y += Random::Float(0.1f, 0.2f) * p.size.y;
            }
            
            p.maxSize       = p.size;
            p.remainingLife = p.lifetime;

            if (++amount == count) break;
        }
    }
}

void ParticleEmitter::create(const Particle& particle, const ParticleInitProperties& property) {

    properties = property;

    uint32_t amount = 0;
    for (Particle& p : particles) {

        if (!p.isActive) {
            
            p = particle;
            p.isActive = true;

            p.position = generate(property);
            p.velocity = Random::vec3(property.velocity_min, property.velocity_max);

            p.acceleration = Random::vec3(property.acc_min, property.acc_max);

            p.size    *= Random::Float(property.size_min,     property.size_max);
            p.lifetime = Random::Float(property.lifetime_min, property.lifetime_max);

            p.maxSize       = p.size;
            p.remainingLife = p.lifetime;

            if (++amount == property.total_count) break;
        }
    }
}

void ParticleEmitter::recreate(Particle& particle) {
    
    particle.position = generate(properties);
    particle.velocity = Random::vec3(properties.velocity_min, properties.velocity_max);
    particle.lifetime = Random::Float(properties.lifetime_min, properties.lifetime_max);

    particle.size = particle.maxSize;
    particle.remainingLife = particle.lifetime;
}

void ParticleEmitter::emit(const glm::vec3& position) {

    uint count = 0;
    // auto position = Random::vec3(0.0f, 0.5f);
    auto lifetime = Random::Float(0.75f, 1.2f);

    for (Particle& p : particles) {

        if (!p.isActive) {

            p.isActive = true;
            p.lifetime = lifetime;
            p.position = position;

            p.color = glm::vec4(1.0f);

            p.remainingLife = p.lifetime;

            //--------------------------------//

            p.remainingLife = Random::Float(0.5f, 1.2f);
            
            p.startColor = Random::vec3(0.0f, 1.0f);
            p.endColor   = Random::vec3(0.0f, 1.0f);

            p.velocity = Random::vec3(10.0f);
            p.size     = glm::vec2(Random::Float(0.1f, 0.2f));
            p.maxSize  = p.size;

            p.acceleration = Random::vec3(2.0f);

            if (++count == 100) break;
        }
    }
}

void ParticleEmitter::update(const float dt) {

    if (Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) particleType = !particleType;

    // if (Input::isKeyPressed(GLFW_KEY_N)) emit(Random::vec3(0.0f, 0.5f));

    // if (!Input::isKeyPressed(GLFW_KEY_N)) return;

    for (Particle& particle : particles) {

        if (!particle.isActive) continue;

        particle.remainingLife -= dt;

        float t = 1.0f - particle.remainingLife / particle.lifetime;
        particle.color = glm::vec4(glm::mix(particle.startColor, particle.endColor, t), 1.0f);

        // particle.size = glm::mix(particle.maxSize, 0.01f, t);

        particle.size.x = glm::mix(particle.maxSize.x, 0.01f, t);
        particle.size.y = glm::mix(particle.maxSize.y, 0.01f, t);

        if (particle.remainingLife <= 0.0f) {

            if (particle.isLooping) {
                recreate(particle);
            }
            else {
                particle.isActive = false;
                continue;
            }
        }

        particle.rotation_angle += particle.angular_velocity * dt;
        if (particle.rotation_angle > 360.0f) particle.rotation_angle -= 360.0f;

        particle.velocity += particle.acceleration * dt;
        particle.position += particle.velocity * dt;
    }
}

void ParticleEmitter::render() const {

    const Shader& shader = *Shaders::get(gfx::shader::TETXURED_PARTICLE);
    // const Shader& shader = *Shaders::get(gfx::shader::COLORED_PARTICLE);
    shader.use();

    const MeshComponent& mesh = EntityShapes::instance().square;

    const glm::mat4& projection = Camera::activeCamera->getProjection();
    const glm::mat4& view       = Camera::activeCamera->getView();

    const glm::vec3 cameraRight(view[0][0], view[1][0], view[2][0]);
    const glm::vec3 cameraUp   (view[0][1], view[1][1], view[2][1]);
    const glm::vec3 cameraFront(view[0][2], view[1][2], view[2][2]);

    glEnable(GL_BLEND);

    for (const Particle& particle : particles) {

        if (!particle.isActive) continue;

        const glm::vec3 up    = glm::normalize(particle.velocity);
        const glm::vec3 right = glm::normalize(glm::cross(up, cameraFront));
        const glm::vec3 front = glm::normalize(glm::cross(right, up));

        const float cos = glm::cos(glm::radians(particle.rotation_angle));
        const float sin = glm::sin(glm::radians(particle.rotation_angle));

        const auto& newRight =  right * cos + up * sin;
        const auto& newUp    = -right * sin + up * cos;

        // Billboarding
        glm::mat4 modelMatrix;

        modelMatrix[0] = glm::vec4(newRight * particle.size.x, 0.0f);
        modelMatrix[1] = glm::vec4(newUp    * particle.size.y, 0.0f);
        modelMatrix[2] = glm::vec4(front,                      0.0f);
        modelMatrix[3] = glm::vec4(particle.position,          1.0f);
        
        if (particleType == gfx::particles::TEXTURED) {

            const Shader& shader = *Shaders::get(gfx::shader::TETXURED_PARTICLE);
            shader.use();

            shader.setMat4("finalMatrix", projection * view * modelMatrix);
            shader.setInt("particle", 0);
            AssetManager::getTexture(particleTexture).bind(0);
        }
        else {

            const Shader& shader = *Shaders::get(gfx::shader::COLORED_PARTICLE);
            shader.use();

            shader.setMat4("finalMatrix", projection * view * modelMatrix);
            shader.setVec4("color", particle.color);
        }

        mesh.draw();
    }
}