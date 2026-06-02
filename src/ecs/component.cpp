#include <ecs/component.h>
#include <debug.h>
#include <ecs/entity.h>

ComponentManager::ComponentManager():

    arr_mesh(MAX_ENTITIES),
    arr_transform(MAX_ENTITIES),
    arr_material(MAX_ENTITIES),
    arr_light(MAX_ENTITIES),
    
    has_mesh(MAX_ENTITIES, false),
    has_transform(MAX_ENTITIES, false),
    has_material(MAX_ENTITIES, false),
    has_light(MAX_ENTITIES, false) {
}

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

void ShapeComponent::draw() const {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
}

void TransformComponent::computeModel() {

    model = glm::translate(glm::mat4(1.0f), position);

    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
}

// ------------------------------ Components ----------------------------------------- //

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
}