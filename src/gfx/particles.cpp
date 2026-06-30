#include <gfx/particles.h>
#include <ecs/entity.h>
#include <math/random.h>
#include <assetManager.h>

#include <shader.h>
#include <camera.h>
#include <input.h>

ParticleEmitter::ParticleEmitter(): particles(MAX_PARTICLES) {
    particleTexture = AssetManager::loadTexture("assets/particles/scorch_01.png", true);
    // particleTexture = AssetManager::loadTexture("assets/textures/pure_red.png");
}

void ParticleEmitter::create() {

    emit(Random::vec3(0.0f, 0.5f));
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
            p.color    = glm::vec4(1.0f);

            p.remainingLife = p.lifetime;

            //--------------------------------//

            p.remainingLife = Random::Float(0.5f, 1.2f);
            
            p.startColor = Random::vec3(0.0f, 1.0f);
            p.endColor   = Random::vec3(0.0f, 1.0f);

            p.velocity = Random::vec3(10.0f);
            p.size     = Random::Float(0.2f);
        }

        if (count == 100) break;
    }
}

void ParticleEmitter::update(const float dt) {

    if (Input::isKeyPressed(GLFW_KEY_N)) create();

    // if (!Input::isKeyPressed(GLFW_KEY_N)) return;

    for (Particle& particle : particles) {

        if (!particle.isActive) continue;

        particle.remainingLife -= dt;

        float t = 1.0f - particle.remainingLife / particle.lifetime;
        particle.color = glm::vec4(glm::mix(particle.startColor, particle.endColor, t), 1.0f);

        particle.size = glm::mix(0.1f, 0.01f, t);

        if (particle.remainingLife <= 0.0f) {
            particle.isActive = false;
            continue;
        }

        particle.position += particle.velocity * dt;
    }
}

void ParticleEmitter::render() const {

    const Shader& shader = *Shaders::getParticleShader();
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

        // Billboarding
        glm::mat4 modelMatrix;
        modelMatrix[0] = glm::vec4(cameraRight * particle.size, 0.0f);
        modelMatrix[1] = glm::vec4(cameraUp    * particle.size, 0.0f);
        modelMatrix[2] = glm::vec4(cameraFront,                 0.0f);
        modelMatrix[3] = glm::vec4(particle.position,           1.0f);

        shader.setVec4("color", particle.color);
        shader.setMat4("finalMatrix", projection * view * modelMatrix);
        
        shader.setInt("particle", 0);
        AssetManager::getTexture(particleTexture).bind(0);

        mesh.draw();
    }
}