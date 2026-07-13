#include <gfx/particles.h>
#include <ecs/entity.h>
#include <math/random.h>
#include <assetManager.h>

#include <shader.h>
#include <camera.h>
#include <input.h>

#include <core/config.h>
#include <debug/assert.h>

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

        Fire.size      = glm::vec2(0.1f);
        Fire.startSize = Fire.size;

        Fire.lifetime = 1.2f;
        Fire.remainingLife = Fire.lifetime;
        
        Fire.rotation_angle   = 0.0f;
        Fire.angular_velocity = 20.0f;

        FireProperties.spawnerType = ParticleSpawnShape::CONE;
        // FireProperties.spawnerType = ParticleSpawnShape::SPHERE;

        FireProperties.center   = { 0.0f, 0.0f, 0.0f };
        // FireProperties.box_size = { 0.2f, 0.2f, 0.2f };
        FireProperties.radius = 0.5f;
        FireProperties.height = -1.0f;

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
        Rain.startSize  = Rain.size;
        
        Rain.lifetime = 3.5f;
        Rain.remainingLife = Rain.lifetime;

        // RainProperties.position_min = {-10.0f,-4.0f,-10.0f };
        // RainProperties.position_max = { 10.0f, 4.0f, 10.0f };

        RainProperties.spawnerType = ParticleSpawnShape::BOX;

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
        flash.startSize = flash.size;

        flash.lifetime = 0.1f;
        flash.remainingLife = flash.lifetime;

        flashProperties.spawnerType = ParticleSpawnShape::BOX;
        
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

        //-------------------------------------------------------------------//

        smoke.acceleration = glm::vec3(
            Random::Float(0.1f),
            0.2f,
            // Random::Float(0.1f)
            -10.0f
        );

        smoke.velocity = glm::vec3(
            0.0f, Random::Float(0.5f, 0.8f), 0.0f
        );

        smoke.startColor = colors::GRAY;
        smoke.endColor   = colors::WHITE;

        smoke.size    = glm::vec2(0.1f);

        // smoke.startSize = glm::vec2(0.0f);
        // smoke.endSize = glm::vec2(0.1f);

        smoke.startSize = glm::vec2(0.1f);
        smoke.endSize = glm::vec2(1.0f);

        smoke.lifetime = 3.0f;
        smoke.remainingLife = smoke.lifetime;
        
        smoke.rotation_angle   = 0.0f;
        smoke.angular_velocity = 0.0f;

        smokeProperties.spawnerType = ParticleSpawnShape::BOX;
        // FireProperties.spawnerType = gfx::particles::SPHERE;

        smokeProperties.center   = { 0.0f, 0.0f, 0.0f };
        smokeProperties.box_size = { 0.1f, 0.1f, 0.1f };
        // FireProperties.radius   = 1.0f;

        smokeProperties.acc_min = smoke.acceleration;
        smokeProperties.acc_max = smoke.acceleration;

        smokeProperties.velocity_min = {-0.1f, 0.5f,-0.1f };
        smokeProperties.velocity_max = { 0.1f, 0.8f, 0.1f };

        // smokeProperties.velocity_min = {-0.1f, 0.5f,-2.1f };
        // smokeProperties.velocity_max = { 0.1f, 0.8f,-2.1f };

        smokeProperties.size_min = 1.0;
        smokeProperties.size_max = 2.0;

        smokeProperties.lifetime_min = smoke.lifetime;
        smokeProperties.lifetime_max = 1.5 * smoke.lifetime;

        smokeProperties.acc_min = glm::vec3(-0.1f, 0.1f,-0.1f);
        smokeProperties.acc_max = glm::vec3( 0.1f, 0.4f, 0.1f);

        //------------------------------- FORCES ------------------------------------//

        wind.direciton = {-0.5f, 0.0f, 0.0f };
        wind.forceType = ParticleForceType::WIND; 

        drag.direciton = glm::vec3(0.5f);
        drag.forceType = ParticleForceType::DRAG;

        gravity.direciton = { 0.0f,-1.0f, 0.0 };
        gravity.forceType = ParticleForceType::GRAVITY;

        buoyancy.direciton = glm::vec3(Random::Float(0.8f, 1.0f));
        buoyancy.forceType = ParticleForceType::BUOYANCY;

        turbulence.direciton = glm::vec3(1.0f);
        turbulence.forceType = ParticleForceType::TURBULENCE;
    }
};

ParticleEmitter::ParticleEmitter():
    particles(MAX_PARTICLES),
    particleTexture(0),
    activeParticles(0),
    particleType(static_cast<bool>(gfx::particles::Type::COLORED))
{
    particleTexture = AssetManager::loadTexture("assets/particles/scorch_01.png", true);

    instanceEmitter.init(MAX_PARTICLES);
}

const glm::vec3 ParticleEmitter::generate(const ParticleInitProperties& spawner) {

    switch (spawner.spawnerType) {

        case ParticleSpawnShape::POINT:
            return spawner.center;
        
        case ParticleSpawnShape::SPHERE: {

            const glm::vec3 direction = Random::vec3(glm::vec3(-1.0f), glm::vec3(1.0f));
            return (spawner.center + glm::normalize(direction) * Random::Float(0, spawner.radius));
        }

        case ParticleSpawnShape::BOX: {

            const glm::vec3 pos_min = -spawner.box_size / 2.0f;
            const glm::vec3 pos_max =  spawner.box_size / 2.0f;

            return (spawner.center + Random::vec3(pos_min, pos_max));
        }

        case ParticleSpawnShape::CONE: {

            bool isInverted = false;
            if (spawner.height < 0.0f) isInverted = true;

            const float height = (isInverted)? 
                Random::Float(-spawner.height, 0.0f) : Random::Float(0.0f, spawner.height);

            const float maxRadius = (isInverted)?
                ((1.0f - height/(-spawner.height)) * spawner.radius) : ((height / spawner.height) * spawner.radius);

            const float angle  = Random::Float(0.0f, glm::two_pi<float>());
            const float radius = maxRadius * glm::sqrt(Random::Float(0.0f, 1.0f));

            return spawner.center + glm::vec3(glm::cos(angle) * radius, height, glm::sin(angle) * radius);
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
            
            p.startSize       = p.size;
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

            p.startSize       = p.size;
            p.remainingLife = p.lifetime;

            if (++amount == property.total_count) break;
        }
    }
}

void ParticleEmitter::recreate(Particle& particle) {
    
    particle.position = generate(properties);
    particle.velocity = Random::vec3(properties.velocity_min, properties.velocity_max);
    particle.lifetime = Random::Float(properties.lifetime_min, properties.lifetime_max);

    particle.size = particle.startSize;
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
            p.startSize  = p.size;

            p.acceleration = Random::vec3(2.0f);

            if (++count == 100) break;
        }
    }
}

void ParticleEmitter::update(const float dt) {

    activeParticles = 0;

    std::vector <gfx::particles::InstancedParticle> instances;

    if (Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) particleType = !particleType;

    // if (Input::isKeyPressed(GLFW_KEY_N)) emit(Random::vec3(0.0f, 0.5f));

    // if (!Input::isKeyPressed(GLFW_KEY_N)) return;

    for (Particle& particle : particles) {

        if (!particle.isActive) continue;

        particle.remainingLife -= dt;

        float t = 1.0f - particle.remainingLife / particle.lifetime;
        particle.color = glm::mix(particle.startColor, particle.endColor, t);

        particle.size.x = glm::mix(particle.startSize.x, particle.endSize.x, t);
        particle.size.y = glm::mix(particle.startSize.y, particle.endSize.y, t);

        particle.alpha = glm::mix(1.0f, 0.0f, t);

        if (particle.remainingLife <= 0.0f) {

            if (isLooping) {
                recreate(particle);
            }
            else {
                particle.isActive = false;
                continue;
            }
        }

        particle.rotation_angle += particle.angular_velocity * dt;
        if (particle.rotation_angle > 360.0f) particle.rotation_angle -= 360.0f;

        for (const auto& force : effects.forces) {

            switch (force.forceType) {

                case gfx::particles::ParticleForceType::WIND: {

                    const glm::vec3 delta = force.direciton - particle.velocity;
                    particle.velocity += delta * glm::length(force.direciton) * dt;
                    break;
                }

                case gfx::particles::ParticleForceType::DRAG: {
                    particle.velocity *= std::exp(-force.direciton.x * dt);
                    break;
                }

                case gfx::particles::ParticleForceType::GRAVITY: {
                    particle.velocity += force.direciton * dt;
                    break;
                }

                case gfx::particles::ParticleForceType::BUOYANCY: {
                    particle.velocity.y += force.direciton.x * dt;
                    break;
                }

                case gfx::particles::ParticleForceType::TURBULENCE: {

                    particle.velocity.x += Random::Float(-force.direciton.x, force.direciton.x) * dt;
                    particle.velocity.z += Random::Float(-force.direciton.x, force.direciton.x) * dt;
                    break;
                }
            }
        }

        particle.velocity += particle.acceleration * dt;
        particle.position += particle.velocity * dt;

        gfx::particles::InstancedParticle inst_particle;
        inst_particle.color    = glm::vec4(particle.color,    particle.alpha);
        inst_particle.position = glm::vec4(particle.position, particle.rotation_angle);
        inst_particle.size     = particle.size;

        instances.push_back(inst_particle);
        activeParticles++;
    }

    instanceEmitter.update(instances);
}

void ParticleEmitter::render(const TextureHandle depthTexture) const {

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    const bool isSoft = (softness > 0.0f);

    if (renderInstance) {

        const Shader& shader = ShaderManager::getUtil(
            (isSoft << 1 | particleType) + gfx::shader::PARTICLE_INSTANCED_SHADER_OFFSET
        );

        const glm::mat4& view = Camera::activeCamera->getView();

        shader.use();
        shader.setMat4("projection", Camera::activeCamera->getProjection());
        shader.setMat4("view", view);

        shader.setVec3("cameraRight", glm::vec3(view[0][0], view[1][0], view[2][0]));
        shader.setVec3("cameraUp",    glm::vec3(view[0][1], view[1][1], view[2][1]));

        if (particleType) {
                
            shader.setInt("particle", 0);
            AssetManager::getTexture(particleTexture).bind(0);
        }

        // Soft Particles
        if (isSoft) {
            
            SNOW_ASSERT(depthTexture != 0, "DEPTH-TEXTURE IS NOT PARSED FOR SOFT-PARTICLES!");
            
            shader.setInt("depthTexture", 1);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            
            shader.setVec2("screenSize", glm::vec2(WIN_W, WIN_H));
            shader.setFloat("nearPlane", Camera::activeCamera->getNearPlane());
            shader.setFloat("farPlane",  Camera::activeCamera->getFarPlane());
            
            shader.setFloat("softness", softness);
        }

        instanceEmitter.draw();
    }
    else {
        const MeshComponent& mesh = EntityShapes::instance().square;
        
        const glm::mat4& projection = Camera::activeCamera->getProjection();
        const glm::mat4& view       = Camera::activeCamera->getView();
        
        const glm::vec3 cameraRight(view[0][0], view[1][0], view[2][0]);
        const glm::vec3 cameraUp   (view[0][1], view[1][1], view[2][1]);
        const glm::vec3 cameraFront(view[0][2], view[1][2], view[2][2]);
        
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
            
            const Shader& shader = ShaderManager::getUtil(
                (isSoft << 1 | particleType) + gfx::shader::PARTICLE_SHADER_OFFSET
            );

            shader.use();
            
            if (particleType) {
                
                shader.setInt("particle", 0);
                AssetManager::getTexture(particleTexture).bind(0);
            }

            // Binded for both COLORED & TEXTURED
            shader.setVec4("uColor", glm::vec4(particle.color, particle.alpha));
            
            // Soft Particles
            if (isSoft) {
                
                SNOW_ASSERT(depthTexture != 0, "DEPTH-TEXTURE IS NOT PARSED FOR SOFT-PARTICLES!");
                
                shader.setInt("depthTexture", 1);
                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_2D, depthTexture);
                
                shader.setVec2("screenSize", glm::vec2(WIN_W, WIN_H));
                shader.setFloat("nearPlane", Camera::activeCamera->getNearPlane());
                shader.setFloat("farPlane",  Camera::activeCamera->getFarPlane());
                
                shader.setFloat("softness", softness);
            }
            
            shader.setMat4("finalMatrix", projection * view * modelMatrix);
            
            mesh.draw();
        }
    }
    
    glDepthMask(GL_TRUE);
}

void InstancedParticles::init(const size_t count) {
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    static const size_t maxSize = sizeof(gfx::particles::InstancedParticle);

    static const std::vector <float> verticesSqr = {

        -0.5f, 0.5f,  0.0f,0.0f,
         0.5f, 0.5f,  1.0f,0.0f,
        -0.5f,-0.5f,  0.0f,1.0f,

         0.5f, 0.5f,  1.0f,0.0f,
         0.5f,-0.5f,  1.0f,1.0f,
        -0.5f,-0.5f,  0.0f,1.0f
    };
    
    glGenBuffers(1, &QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSqr.size() * sizeof(float), verticesSqr.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TextureCords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * maxSize, nullptr, GL_DYNAMIC_DRAW);

    // Color
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, maxSize, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Position
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, maxSize, (void*)(offsetof(gfx::particles::InstancedParticle, position)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // Size
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, maxSize, (void*)(offsetof(gfx::particles::InstancedParticle, size)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InstancedParticles::update(const std::vector<gfx::particles::InstancedParticle>& particles) {

    total_count = particles.size();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, total_count * sizeof(gfx::particles::InstancedParticle), particles.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedParticles::draw() const {

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, total_count);
    glBindVertexArray(0);
}