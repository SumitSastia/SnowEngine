#include <entity.h>

#include <camera.h>
#include <shader.h>
#include <shapes.h>
#include <renderer.h>

// ------------------------------ ECS Renderer --------------------------------------- //\

void RenderSystem::bindCameraGlobals(const Shader* shader) const {

    shader->setMat4("projection", Camera::instance().getPerspective());
    shader->setMat4("view",       Camera::instance().getView());
}

void RenderSystem::render(const ECS& ecs) const {

    for (Entity entity = 0; entity < ecs.getTotal(); entity++) {

        const Transform&  transform = ecs.transforms[entity];
        const EntityMesh& mesh      = ecs.meshes[entity];
        const Material&   material  = ecs.materials[entity];

        draw(mesh, transform, material);
    }
}

void RenderSystem::draw(const EntityMesh& mesh, const Transform& transform, const Material& material) const {

    material.shader->use();

    material.shader->setMat4("model",        transform.model);
    material.shader->setMat4("normalMatrix", transform.normalMatrix);

    bindCameraGlobals(material.shader);

    mesh.shape->draw();
}

// ------------------------------ ECS Renderer --------------------------------------- //

void Transform::computeModel() {

    model = glm::translate(glm::mat4(1.0f), position);

    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
}

EntityShapes::EntityShapes() {

    cube = ecs.createEntity();
    
    // Mesh
    ecs.meshes[cube].shape = &DefaultShapes::instance().cube;
    
    // Material
    ecs.materials[cube].shader = new Shader(
        "../shaders/ecs/cube.vert",
        "../shaders/ecs/cube.frag"
    );
    
    // Transform
    Transform transform {};

    transform.position = glm::vec3(-3.0f, 0.0f, 3.0f);
    transform.rotation = glm::vec3(0.0f);
    transform.scale    = glm::vec3(1.0f);

    transform.computeModel();
    ecs.transforms[cube] = transform;
}