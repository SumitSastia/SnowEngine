#include <gfx/particles.h>
#include <ecs/entity.h>
#include <math/random.h>
#include <assetManager.h>

#include <shader.h>
#include <camera.h>
#include <input.h>

namespace gfx::particles {
    
    void init() {

        Fire.acceleration = glm::vec3(
            Random::Float(0.2f),
            0.2f,
            Random::Float(0.2f)
        );

        Fire.velocity   = glm::vec3(0.0f, Random::Float(0.5f, 0.8f), 0.0f);
        Fire.startColor = colors::ORANGE;
        Fire.endColor   = colors::YELLOW;

        // Fire.startColor = colors::BLUE;
        // Fire.endColor   = colors::LIGHT_BLUE;

        Fire.size     = glm::vec2(0.1f);
        Fire.maxSize  = Fire.size;
        Fire.lifetime = 1.2f;

        Fire.remainingLife = Fire.lifetime;

        // Based on Origin {0,0,0}
        FireProperties.position_min = glm::vec3(-0.1f);
        FireProperties.position_max = glm::vec3( 0.1f);

        FireProperties.velocity_min = {-0.1f, 0.5f,-0.1f };
        FireProperties.velocity_max = { 0.1f, 0.8f, 0.1f };

        FireProperties.size_min = 1.0f;
        FireProperties.size_max = 1.5f;

        FireProperties.lifetime_min = 1.0f;
        FireProperties.lifetime_max = 3.0f;

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
        Rain.lifetime = 4.0f;

        Rain.remainingLife = Rain.lifetime;
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

void ParticleEmitter::create(const Particle& particle, const glm::vec3& velocity_variation, const uint32_t count) {

    uint32_t amount = 0;
    for (Particle& p : particles) {

        if (!p.isActive) {
            
            p = particle;
            p.isActive = true;

            p.velocity = p.velocity + Random::Float(-1.0f, 1.0f) * velocity_variation;

            if (++amount == count) break;
        }
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

void ParticleEmitter::create(const Particle& particle, const ParticleInitProperties& property, const uint32_t count) {

    uint32_t amount = 0;
    for (Particle& p : particles) {

        if (!p.isActive) {
            
            p = particle;
            p.isActive = true;

            p.position = Random::vec3(property.position_min, property.position_max);
            p.velocity = Random::vec3(property.velocity_min, property.velocity_max);

            p.size    *= Random::Float(property.size_min,     property.size_max);
            p.lifetime = Random::Float(property.lifetime_min, property.lifetime_max);

            p.maxSize       = p.size;
            p.remainingLife = p.lifetime;

            if (++amount == count) break;
        }
    }
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
            particle.isActive = false;
            continue;
        }

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

        // Billboarding
        glm::mat4 modelMatrix;
        // modelMatrix[0] = glm::vec4(cameraRight * particle.size.x, 0.0f);
        // modelMatrix[1] = glm::vec4(cameraUp    * particle.size.y, 0.0f);
        // modelMatrix[2] = glm::vec4(cameraFront,                 0.0f);
        // modelMatrix[3] = glm::vec4(particle.position,           1.0f);

        modelMatrix[0] = glm::vec4(right * particle.size.x, 0.0f);
        modelMatrix[1] = glm::vec4(up    * particle.size.y, 0.0f);
        modelMatrix[2] = glm::vec4(front,                   0.0f);
        modelMatrix[3] = glm::vec4(particle.position,       1.0f);
        
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