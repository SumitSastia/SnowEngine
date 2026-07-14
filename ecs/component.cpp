#include "entity.h"
#include "component.h"

#include "core/camera.h"
#include "core/model/model.h"
#include "core/shader/shader.h"
#include "core/frame/frame.h"
#include "core/img/texture.h"

#include "utils/debug.h"

// ------------------------------ Temporary ------------------------------------------ //

// ------------------------------ Components ----------------------------------------- //

void CameraComponent::renderFrustum() const {

    glm::vec4 NDC_corners[8] = {

        // Near Plane
        {-1,-1,-1, 1},
        { 1,-1,-1, 1},
        { 1, 1,-1, 1},
        {-1, 1,-1, 1},

        // Far Plane
        {-1,-1, 1, 1},
        { 1,-1, 1, 1},
        { 1, 1, 1, 1},
        {-1, 1, 1, 1}
    };

    glm::vec3 corners[8];

    const glm::mat4 invVP = glm::inverse(camera.getProjection() * camera.getView());

    for (uint i = 0; i < 8; i++) {

        glm::vec4 p = invVP * NDC_corners[i];
        corners[i]  = glm::vec3(p) / p.w;
        corners[i]  = camera.getPos() + (corners[i] - camera.getPos()) * 0.01f;
    }

    // Near Plane Quad
    Line::render(corners[0], corners[1]);
    Line::render(corners[1], corners[2]);
    Line::render(corners[2], corners[3]);
    Line::render(corners[3], corners[0]);

    // Slanted Edges
    Line::render(corners[4], corners[5]);
    Line::render(corners[5], corners[6]);
    Line::render(corners[6], corners[7]);
    Line::render(corners[7], corners[4]);

    // Far Plane Quad
    Line::render(corners[0], corners[4]);
    Line::render(corners[1], corners[5]);
    Line::render(corners[2], corners[6]);
    Line::render(corners[3], corners[7]);
}

void MeshComponent::loadMesh2D(
    const std::vector<float>& vertices,
    const std::vector<uint>&  indices
) {
    indicesCount = indices.size();

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TextureCords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MeshComponent::loadMesh3D(
    const std::vector<float>& vertices,
    const std::vector<uint>&  indices
) {
    indicesCount = indices.size();

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint), indices.data(), GL_STATIC_DRAW);

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

void MeshComponent::loadMesh3D(
    const std::vector<Vertex>& vertices,
    const std::vector<uint>&  indices
) {
    indicesCount = indices.size();

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, textureCords));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MeshComponent::loadMesh3D(
    const std::vector<Vertex_n>& vertices,
    const std::vector<uint>&     indices
) {
    indicesCount = indices.size();

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_n), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)offsetof(Vertex_n, normal));
    glEnableVertexAttribArray(1);

    // Tangent
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)offsetof(Vertex_n, bitangent));
    glEnableVertexAttribArray(2);

    // TextureCords
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)offsetof(Vertex_n, textureCords));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MeshComponent::loadMesh3DNormal(
    const std::vector<float>& vertices,
    const std::vector<uint>&  indices
) {
    indicesCount = indices.size();

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Tangent
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // TextureCords
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MeshComponent::draw() const {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void MeshComponent::destroy() {

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    VBO = 0;
    EBO = 0;
    VAO = 0;
}

void InstanceComponent::bind(
    const std::vector<float>& vertices,
    const std::vector<uint>&  indices
) {
    mesh.indicesCount = indices.size();
    visibleCount = count;

    if (!count) DebugMenu::log("ERROR::TRANSFORM-COMPONENTS NOT INITIALIZED!");

    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenVertexArrays(1, &mesh.VAO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // TextureCords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    std::vector <glm::mat4> models(count);
    std::vector <glm::mat3> normals(count);

    for (uint32_t i = 0; i < count; i++) {

        models[i]  = transforms[i].model;
        normals[i] = transforms[i].model.getNormal();
    }

    // InstancedModel
    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat3), normals.data(), GL_STATIC_DRAW);

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

void InstanceComponent::updateModels() {

    visibleCount = visibleInstances.size();

    std::vector <glm::mat4> models(visibleCount);
    std::vector <glm::mat3> normals(visibleCount);
    
    for (uint32_t i = 0; i < visibleCount; i++) {
        
        models[i]  = visibleInstances[i]->model;
        normals[i] = visibleInstances[i]->model.getNormal();
    }
    
    glBindVertexArray(mesh.VAO);

    // InstancedModel
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, visibleCount * sizeof(glm::mat4), models.data());
    
    // InstancedNormal
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, visibleCount * sizeof(glm::mat3), normals.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InstanceComponent::draw() const {

    glBindVertexArray(mesh.VAO);
    glDrawElementsInstanced(GL_TRIANGLES, mesh.indicesCount, GL_UNSIGNED_INT, nullptr, visibleCount);
    glBindVertexArray(0);
}

void InstanceComponent::destroy() {

    mesh.destroy();

    glDeleteBuffers(1, &modelVBO);
    glDeleteBuffers(1, &normalVBO);
}

void TransformComponent::computeModel() {

    model.matrix_4x4 = glm::translate(glm::mat4(1.0f), position);

    model.matrix_4x4 = glm::rotate(model.matrix_4x4, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model.matrix_4x4 = glm::rotate(model.matrix_4x4, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model.matrix_4x4 = glm::rotate(model.matrix_4x4, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model.matrix_4x4 = glm::scale(model.matrix_4x4, scale);
}

const MeshComponent& MeshLODComponent::getMesh(const float distance) const {

    if (distance < 5.0f)  return high;
    if (distance < 10.0f) return moderate;
    return low;
}

void MeshLODComponent::destroy() {
    high.destroy();
    moderate.destroy();
    low.destroy();
}

void ModelComponent::destroy() {

    for (MeshComponent& mesh : meshes) {
        mesh.destroy();
    }
}

const ModelComponent& ModelLODComponent::getMesh(const float distance) const {

    if (distance < 5.0f)  return high;
    if (distance < 10.0f) return moderate;
    return low;
}

void ModelLODComponent::destroy() {
    high.destroy();
    moderate.destroy();
    low.destroy();
}

void BoundingAABBComponent::recompute(const glm::mat4& model) {

    center = model[3];

    glm::vec3 corners[8] = {
        {local_min.x, local_min.y, local_min.z},
        {local_min.x, local_min.y, local_max.z},
        {local_min.x, local_max.y, local_min.z},
        {local_min.x, local_max.y, local_max.z},
        {local_max.x, local_min.y, local_min.z},
        {local_max.x, local_min.y, local_max.z},
        {local_max.x, local_max.y, local_min.z},
        {local_max.x, local_max.y, local_max.z}
    };

    min = glm::vec3(FLT_MAX);
    max = glm::vec3(-FLT_MAX);

    for (glm::vec3& corner : corners) {

        corner = glm::vec3(model * glm::vec4(corner, 1.0f));
        
        min = glm::min(min, corner);
        max = glm::max(max, corner);
    }
}

void ChildComponent::destroy() {

    for (const Entity& child : children) {

    }
}

// ------------------------------ Components ----------------------------------------- //

template <>
ComponentPool<TransformComponent>& ComponentManager::getPool() {
    return transforms;
}

template <>
ComponentPool<MeshComponent>& ComponentManager::getPool() {
    return meshes;
}

template <>
ComponentPool<MaterialComponent>& ComponentManager::getPool() {
    return materials;
}

template <>
ComponentPool<InstanceComponent>& ComponentManager::getPool() {
    return instances;
}

template <>
ComponentPool<ModelComponent>& ComponentManager::getPool() {
    return models;
}

template <>
ComponentPool<PointLightComponent>& ComponentManager::getPool() {
    return pointlights;
}

template <>
ComponentPool<DirectionalLight>& ComponentManager::getPool() {
    return directlights;
}

template <>
ComponentPool<BoundingSphereComponent>& ComponentManager::getPool() {
    return boundingSpheres;
}

template <>
ComponentPool<BoundingAABBComponent>& ComponentManager::getPool() {
    return boundingAABBs;
}

template <>
ComponentPool<MeshLODComponent>& ComponentManager::getPool() {
    return meshLODs;
}

template <>
ComponentPool<ModelLODComponent>& ComponentManager::getPool() {
    return modelLODs;
}

template <>
ComponentPool<CameraComponent>& ComponentManager::getPool() {
    return cameras;
}

template <>
ComponentPool<ChildComponent>& ComponentManager::getPool() {
    return childrens;
}

template <>
ComponentPool<SpotLightComponent>& ComponentManager::getPool() {
    return spotlights;
}

template <>
ComponentPool<AnimatedSprite>& ComponentManager::getPool() {
    return animSprites;
}

template <>
const ComponentPool<TransformComponent>& ComponentManager::getPool() const {
    return transforms;
}

template <>
const ComponentPool<MeshComponent>& ComponentManager::getPool() const {
    return meshes;
}

template <>
const ComponentPool<MaterialComponent>& ComponentManager::getPool() const {
    return materials;
}

template <>
const ComponentPool<InstanceComponent>& ComponentManager::getPool() const {
    return instances;
}

template <>
const ComponentPool<ModelComponent>& ComponentManager::getPool() const {
    return models;
}

template <>
const ComponentPool<PointLightComponent>& ComponentManager::getPool() const {
    return pointlights;
}

template <>
const ComponentPool<DirectionalLight>& ComponentManager::getPool() const {
    return directlights;
}

template <>
const ComponentPool<BoundingSphereComponent>& ComponentManager::getPool() const {
    return boundingSpheres;
}

template <>
const ComponentPool<BoundingAABBComponent>& ComponentManager::getPool() const {
    return boundingAABBs;
}

template <>
const ComponentPool<MeshLODComponent>& ComponentManager::getPool() const {
    return meshLODs;
}

template <>
const ComponentPool<ModelLODComponent>& ComponentManager::getPool() const {
    return modelLODs;
}

template <>
const ComponentPool<CameraComponent>& ComponentManager::getPool() const {
    return cameras;
}

template <>
const ComponentPool<ChildComponent>& ComponentManager::getPool() const {
    return childrens;
}

template <>
const ComponentPool<SpotLightComponent>& ComponentManager::getPool() const {
    return spotlights;
}

template <>
const ComponentPool<AnimatedSprite>& ComponentManager::getPool() const {
    return animSprites;
}

// ------------------------------ Components ----------------------------------------- //

void ComponentManager::addShader(const ShaderHandle shader) {

    bool skip = false;

    for (const ShaderHandle handle : uniqueShaders) {
        if (shader == handle) { skip = true; break; }
    }

    if (!skip) uniqueShaders.push_back(shader);
}

template <>
void ComponentManager::addComponent<MaterialComponent>(const Entity& entity, const MaterialComponent& component) {

    getPool<MaterialComponent>().addComponent(entity, component);
    addShader(component.shader);
}

template <>
void ComponentManager::addComponent<PointLightComponent>(const Entity& entity, const PointLightComponent& component) {

    getPool<PointLightComponent>().addComponent(entity, component);

    pointShadowFrames.push_back(
        PointShadowData(entity, new PointShadowFrame())
    );
}