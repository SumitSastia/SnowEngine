#include <gfx/particles.h>
#include <ecs/entity.h>

#include <shader.h>
#include <camera.h>

void ParticleEmitter::create(const Particle& particle) {

    for (Particle& p : particles) {

        if (!p.isActive) {

            p = particle;
            p.isActive = true;
            p.remainingLife = p.lifetime;

            break;
        }
    }
}

void ParticleEmitter::emit() {

}

void ParticleEmitter::update(const float dt) {

    for (Particle& particle : particles) {

        if (!particle.isActive) continue;

        particle.remainingLife -= dt;

        if (particle.remainingLife <= 0.0f) {
            particle.isActive = false;
            continue;
        }

        particle.position += particle.velocity * dt;
    }
}

void ParticleEmitter::render() const {

    const Shader shader = *Shaders::getParticleShader();
    shader.use();

    const MeshComponent& mesh = EntityShapes::instance().square;

    const glm::mat4& projection = Camera::activeCamera->getPerspective();
    const glm::mat4& view       = Camera::activeCamera->getView();

    for (const Particle& particle : particles) {

        Matrix4 modelMatrix;
        modelMatrix.translate(particle.position);
        modelMatrix.scale(particle.size);

        shader.setVec4("color", particle.color);
        shader.setMat4("finalMatrix", projection * view * modelMatrix.getMatrix());
        mesh.draw();
    }
}