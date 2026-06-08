#include <ecs/component.h>
#include <debug.h>
#include <ecs/entity.h>
#include <model.h>
#include <shader.h>
#include <frame.h>

// ------------------------------ Components ----------------------------------------- //

void ShapeComponent::bindVertices2D(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(u_int);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TextureCords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void ShapeComponent::bindVertices3D(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(u_int);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ShapeComponent::bindVertices3D_Normal(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(u_int);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ShapeComponent::draw() const {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
}

void InstanceComponent::bindVertices(
    const float*        vertices, const size_t& size_v,
    const unsigned int* indices,  const size_t& size_i
) {
    indicesCount = size_i / sizeof(u_int);

    if (!count) DebugMenu::log("ERROR::TRANSFORMCOMPONENTS NOT INITIALIZED!");

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    std::vector <glm::mat4> models(count);
    std::vector <glm::mat3> normals(count);

    for (uint32_t i = 0; i < count; i++) {

        models[i]  = transforms[i].model;
        normals[i] = transforms[i].model.getNormal();
    }

    // InstancedModel
    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), &models[0], GL_STATIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glVertexAttribDivisor(3, 1);
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glVertexAttribDivisor(4, 1);
    
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glVertexAttribDivisor(5, 1);
    
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
    glVertexAttribDivisor(6, 1);
    
    // InstancedNormal
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat3), &normals[0], GL_STATIC_DRAW);

    std::size_t vec3Size = sizeof(glm::vec3);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)0);
    glVertexAttribDivisor(7, 1);
    
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)(1 * vec3Size));
    glVertexAttribDivisor(8, 1);
    
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)(2 * vec3Size));
    glVertexAttribDivisor(9, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InstanceComponent::draw() const {

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, count);
    glBindVertexArray(0);
}

void TransformComponent::computeModel() {

    model.matrix_4x4 = glm::translate(glm::mat4(1.0f), position);

    model.matrix_4x4 = glm::rotate(model.matrix_4x4, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model.matrix_4x4 = glm::rotate(model.matrix_4x4, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model.matrix_4x4 = glm::rotate(model.matrix_4x4, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model.matrix_4x4 = glm::scale(model.matrix_4x4, scale);
}

void ModelComponent::init(const Model3D* model, Shader* shader, Shader* gbufferShader) {

    for (const Mesh& mesh : model->meshes) {
        
        MeshComponent meshComponent;
        meshComponent.shape = ShapeComponent(
            mesh.VAO, mesh.VBO, mesh.EBO,
            mesh.indices.size()
        );

        MaterialComponent material;
        material.shader = shader;
        material.albedo = new Texture2D(mesh.textures[0].id);

        material.gbufferShader = gbufferShader;

        meshes.push_back(meshComponent);
        materials.push_back(material);
    }
}

// ------------------------------ Components ----------------------------------------- //

ComponentManager::ComponentManager():

    arr_mesh(MAX_ENTITIES),
    arr_transform(MAX_ENTITIES),
    arr_material(MAX_ENTITIES),
    arr_light(MAX_ENTITIES),
    arr_directionalLight(MAX_ENTITIES),
    arr_instance(MAX_ENTITIES),
    arr_model(MAX_ENTITIES),
    
    has_mesh(MAX_ENTITIES, false),
    has_transform(MAX_ENTITIES, false),
    has_material(MAX_ENTITIES, false),
    has_light(MAX_ENTITIES, false),
    has_directionalLight(MAX_ENTITIES, false),
    has_instance(MAX_ENTITIES, false),
    has_model(MAX_ENTITIES, false) {
}

void ComponentManager::addShader(Shader* shader) {

    bool skip = false;
    for (const Shader* s : uniqueShaders) {
        if (shader == s) { skip = true; break; }
    }

    if (!skip) {
        uniqueShaders.push_back(shader);
    }
}

template <typename Component>
void ComponentManager::addComponent(const Entity& entity, const Component& component) {
    DebugMenu::log("ERROR::THIS COMPONENT IS NOT INITIALIZED IN THE COMPONENT MANAGER!");
}

template <>
void ComponentManager::addComponent<TransformComponent>(const Entity& entity, const TransformComponent& component) {

    arr_transform[entity] = component;
    has_transform[entity] = true;
}

template <>
void ComponentManager::addComponent<MeshComponent>(const Entity& entity, const MeshComponent& component) {

    arr_mesh[entity] = component;
    has_mesh[entity] = true;
}

template <>
void ComponentManager::addComponent<MaterialComponent>(const Entity& entity, const MaterialComponent& component) {

    arr_material[entity] = component;
    has_material[entity] = true;

    addShader(component.shader);
}

template <>
void ComponentManager::addComponent<PointLightComponent>(const Entity& entity, const PointLightComponent& component) {

    arr_light[entity] = component;
    has_light[entity] = true;

    pointShadowFrames.push_back(
        PointShadowData(entity, new PointShadowFrame())
    );
}

template <>
void ComponentManager::addComponent<InstanceComponent>(const Entity& entity, const InstanceComponent& component) {

    arr_instance[entity] = component;
    has_instance[entity] = true;
}

template <>
void ComponentManager::addComponent<ModelComponent>(const Entity& entity, const ModelComponent& component) {

    arr_model[entity] = component;
    has_model[entity] = true;
}