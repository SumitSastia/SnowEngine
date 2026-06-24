#include <systems/rendersystem.h>
#include <assetManager.h>

#include <shader.h>
#include <shapes.h>
#include <renderer.h>
#include <debug.h>
#include <lights.h>
#include <frame.h>
#include <input.h>
#include <frustum.h>

#include <iostream>

void RenderSystem::bindCameraGlobals(const Shader* shader) {

    shader->setMat4("projection", Camera::get_projection());
    shader->setMat4("view",       Camera::get_view());
    shader->setVec3("camPos",     Camera::get_position());
}

void RenderSystem::bindPointLightGlobals(const ECS& ecs) {

    const EntityManager&    entityManager    = ecs.entityManager;
    const ComponentManager& componentManager = ecs.componentManager;

    const uint32_t light_count = entityManager.emissiveEntities.size();

    // Global Textures Binding
    
    for (const Shader* shader : componentManager.uniqueShaders) {
        
        shader->use();

        lastTextureUnit = 0;

        // ----------------------- SpotLight ----------------------- //

        shader->setBool("useSpotLight", DefaultLights::instance().flashlight.isVisible);
        shader->setSpotLight("sl", DefaultLights::instance().flashlight);

        // ------------------------ Camera ------------------------- //
        
        bindCameraGlobals(shader);

        // ----------------------- PointLight ---------------------- //

        shader->setInt("light_count", light_count);
        shader->setFloat("far_plane", 25.0f);

        for (uint32_t index = 0; index < light_count; index++) {

            const Entity& entity = entityManager.emissiveEntities[index];

            if (
                componentManager.has<PointLightComponent>(entity) &&
                componentManager.has<TransformComponent>(entity)
            ) {
                const PointLightComponent& pointlight  = componentManager.get<PointLightComponent>(entity);
                const TransformComponent&  transform   = componentManager.get<TransformComponent>(entity);

                const std::string e  = "pl[" + std::to_string(index) + "]";

                shader->setVec3((e + ".position").c_str(), transform.position);
                shader->setVec3((e + ".color").c_str(),    pointlight.color);

                shader->setFloat((e + ".constant").c_str(),  pointlight.constant);
                shader->setFloat((e + ".linear").c_str(),    pointlight.linear);
                shader->setFloat((e + ".quadratic").c_str(), pointlight.quadratic);
            }
        }

        // ------------------------ DepthMap ----------------------- //

        for (uint8_t i = 0; i < MAX_LIGHTS; i++) {

            if (i < light_count) componentManager.pointShadowFrames[i].frame->bindTexture(lastTextureUnit);
            shader->setInt(("depthMap[" + std::to_string(i) + "]").c_str(), lastTextureUnit++);
        }

        // --------------------- Directional Light ----------------- //

        shader->setDirectionalLight("dl", DefaultLights::instance().sunlight);
        shader->setBool("useDirectionalLight", Input::isKeyPressed(GLFW_KEY_L));
        // shader->setBool("useDirectionalLight", true);

        shader->setInt("dl_depthMap", lastTextureUnit);
        componentManager.directShadowFrames[0].frame->bindTexture(lastTextureUnit++);

        shader->setMat4("lightSpaceMatrix", componentManager.directShadowFrames[0].lightSpaceMatrix);
        
        // ---------------------- Environment ---------------------- //

        shader->setInt("irradianceMap", lastTextureUnit);
        entityManager.env->bindIrradiance(lastTextureUnit++);

        shader->setBool("useIrradiance", Input::isKeyPressed(GLFW_KEY_B));
        // shader->setBool("useIrradiance", true);

        shader->setInt("preFilterMap", lastTextureUnit);
        // entityManager.env->bindPrefilter(lastTextureUnit++);
        entityManager.env->bindTexture(lastTextureUnit++);

        shader->setInt("brdfLUT", lastTextureUnit);
        entityManager.env->bindBRDF(lastTextureUnit++);
    }
}

void RenderSystem::update(const float deltaTime) {

}

void RenderSystem::render(const ECS& ecs) {
    
    bindPointLightGlobals(ecs);

    const EntityManager&    entityManager    = ecs.entityManager;
    const ComponentManager& componentManager = ecs.componentManager;

    const std::vector<Entity> objects   = ecs.view<MeshComponent, TransformComponent, MaterialComponent>();
    const std::vector<Entity> instances = ecs.view<InstanceComponent, MaterialComponent>();
    const std::vector<Entity> models    = ecs.view<ModelComponent, TransformComponent, MaterialComponent>();

    const std::vector<Entity> objects_with_LOD = ecs.view<MeshLODComponent, TransformComponent, MaterialComponent>();
    const std::vector<Entity> models_with_LOD  = ecs.view<ModelLODComponent, TransformComponent, MaterialComponent>();

    // Frustum
    Frustum frustum(Camera::get_projection() * Camera::get_view());

    int totalRenderCalls = 0;
    for (const Entity& entity : objects) {

        // Temporary Fix: Prevents Light Meshes to render
        if (componentManager.has<PointLightComponent>(entity)) continue;

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const TransformComponent&    transform    = componentManager.get<TransformComponent>(entity);
        if (!transform.isVisible) continue;

        const MeshComponent&      mesh      = componentManager.get<MeshComponent>(entity);
        const MaterialComponent&  material  = componentManager.get<MaterialComponent>(entity);

        draw(mesh, transform, material);

        drawWireframe(Wireframes::instance().cube, componentManager.get<BoundingAABBComponent>(entity));
        totalRenderCalls++;
    }

    // DebugMenu::log("Total Render Calls: " + std::to_string(totalRenderCalls));

    for (const Entity& entity : instances) {

        // Without Frustum Culling
        // const InstanceComponent& instance = componentManager.get<InstanceComponent>(entity);
        // const MaterialComponent& material = componentManager.get<MaterialComponent>(entity);

        // draw(instance, material);

        // With Frustum Culling
        InstanceComponent instance = componentManager.get<InstanceComponent>(entity);
        const MaterialComponent& material = componentManager.get<MaterialComponent>(entity);

        instance.visibleInstances.clear();

        for (uint32_t i = 0; i < instance.count; i++) {

            if (frustum.isMeshInside(instance.AABBs[i])) {
                instance.visibleInstances.push_back(&instance.transforms[i]);
            }

            drawWireframe(Wireframes::instance().cube, instance.AABBs[i]);
        }

        instance.updateModels();

        // DebugMenu::log("Instances: " + std::to_string(instance.visibleCount));
        draw(instance, material);
    }

    for (const Entity& entity : models) {

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const ModelComponent&     model     = componentManager.get<ModelComponent>(entity);
        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);

        if (!transform.isVisible) continue;
        const uint total_meshes = model.meshes.size();

        MaterialComponent material = componentManager.get<MaterialComponent>(entity);

        for (uint i = 0 ; i < total_meshes; i++) {

            const MeshComponent& mesh = model.meshes[i];
            material.albedo = model.textures[i];

            draw(mesh, transform, material);
        }

        drawWireframe(Wireframes::instance().cube, componentManager.get<BoundingAABBComponent>(entity));
    }
    
    // LOD System
    for (const Entity& entity : objects_with_LOD) {

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
        const MaterialComponent&  material  = componentManager.get<MaterialComponent>(entity);
        
        if (!transform.isVisible) continue;
        float distance = glm::length(Camera::instance().getPos() - transform.position);
        const MeshComponent& mesh = componentManager.get<MeshLODComponent>(entity).getMesh(distance);

        draw(mesh, transform, material);
    }

    for (const Entity& entity : models_with_LOD) {

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
        MaterialComponent material   = componentManager.get<MaterialComponent>(entity);
        
        float distance = glm::length(Camera::instance().getPos() - transform.position);
        const ModelComponent& model = componentManager.get<ModelLODComponent>(entity).getMesh(distance);

        const uint total_meshes = model.meshes.size();

        for (uint i = 0 ; i < total_meshes; i++) {

            const MeshComponent& mesh = model.meshes[i];
            material.albedo = model.textures[i];

            draw(mesh, transform, material);
        }

        if (componentManager.has<BoundingAABBComponent>(entity)) {
            drawWireframe(Wireframes::instance().cube, componentManager.get<BoundingAABBComponent>(entity));
        }
    }
}

void RenderSystem::renderLights(const ECS& ecs) {

    const EntityManager&    entityManager    = ecs.entityManager;
    const ComponentManager& componentManager = ecs.componentManager;

    Frustum frustum(Camera::get_projection() * Camera::get_view());

    for (const Entity& entity : entityManager.emissiveEntities) {

        if (componentManager.has<PointLightComponent>(entity)) {

            const MeshComponent&       mesh       = componentManager.get<MeshComponent>(entity);
            const TransformComponent&  transform  = componentManager.get<TransformComponent>(entity);
            const MaterialComponent&   material   = componentManager.get<MaterialComponent>(entity);
            const PointLightComponent& pointlight = componentManager.get<PointLightComponent>(entity);
            
            draw(mesh, transform, material, pointlight);
        }
    }
}

void RenderSystem::drawWireframe(
    const gfx::internal::Wireframe& wireframe,
    const BoundingSphereComponent&  sphere
) {

    static bool toggle = false;
    if (Input::isKeyDown(GLFW_KEY_Y)) toggle = !toggle;

    if (toggle) {
        const Shader& shader = *Shaders::get(WIREFRAME);
    
        shader.use();
        bindCameraGlobals(&shader);
    
        Matrix4 model;
        model.translate(sphere.center);
        model.scale(glm::vec3(sphere.radius + 0.01f));
    
        shader.setMat4("model", model);
    
        wireframe.draw();
    }
}

void RenderSystem::drawWireframe(
    const gfx::internal::Wireframe& wireframe,
    const BoundingAABBComponent&    AABB
) {

    static bool toggle = false;
    if (Input::isKeyDown(GLFW_KEY_Y)) toggle = !toggle;

    if (toggle) {
        const Shader& shader = *Shaders::get(WIREFRAME);
    
        shader.use();
        bindCameraGlobals(&shader);
    
        Matrix4 model;
        model.translate(AABB.center);
        model.scale(AABB.max - AABB.min);
    
        shader.setMat4("model", model);
    
        wireframe.draw();
    }
}

void RenderSystem::draw(
    const MeshComponent&      mesh,
    const TransformComponent& transform,
    const MaterialComponent&  material
) {
    const Shader* shader = material.shader;

    shader->use();
    shader->setMat4("model",        transform.model);
    shader->setMat3("normalMatrix", transform.model.getNormal());

    bindCameraGlobals(shader);

    const uint32_t initialUnit = lastTextureUnit;

    if (material.albedo) {
        shader->setInt("albedo", initialUnit);
        AssetManager::getTexture(material.albedo).bind(initialUnit);
        // AssetManager::getTexture(0).bind(initialUnit);
    }

    if (material.normal) {
        shader->setInt("normalMap", initialUnit+1);
        AssetManager::getTexture(material.normal).bind(initialUnit + 1);
    }

    if (material.height) {
        shader->setFloat("height_scale", 0.1);
        shader->setInt("heightMap", initialUnit+2);
        AssetManager::getTexture(material.height).bind(initialUnit + 2);
    }

    if (material.metallic) {
        shader->setInt("metallicMap", initialUnit+3);
        AssetManager::getTexture(material.metallic).bind(initialUnit + 3);
    }

    if (material.roughness) {
        shader->setInt("roughnessMap", initialUnit+4);
        AssetManager::getTexture(material.roughness).bind(initialUnit + 4);
    }

    if (material.specular) {
        shader->setInt("specularMap", initialUnit+5);
        AssetManager::getTexture(material.specular).bind(initialUnit + 5);
    }

    mesh.draw();
}

void RenderSystem::draw(
    const MeshComponent&       mesh,
    const TransformComponent&  transform,
    const MaterialComponent&   material,
    const PointLightComponent& pointlight
) {
    const Shader* shader = material.shader;

    bindCameraGlobals(shader);

    shader->use();
    shader->setMat4("model",      transform.model);
    shader->setVec3("lightColor", pointlight.color);

    // mesh.shape.draw();
    mesh.draw();
}

void RenderSystem::draw(
    const InstanceComponent& instance,
    const MaterialComponent& material
) {

    material.shader->use();

    const uint32_t initialUnit = lastTextureUnit;

    if (material.albedo) {
        material.shader->setInt("albedo", initialUnit);
        AssetManager::getTexture(material.albedo).bind(initialUnit);
    }

    if (material.normal) {
        material.shader->setInt("normalMap", initialUnit+1);
        AssetManager::getTexture(material.normal).bind(initialUnit + 1);
    }

    instance.draw();
}

void RenderSystem::drawGbuffer(
    const MeshComponent&      mesh,
    const TransformComponent& transform,
    const MaterialComponent&  material
) {
    const Shader* shader = material.gbufferShader;

    shader->use();
    shader->setMat4("model",        transform.model);
    shader->setMat3("normalMatrix", transform.model.getNormal());

    bindCameraGlobals(shader);

    const uint32_t initialUnit = lastTextureUnit;

    if (material.albedo) {
        shader->setInt("albedo", initialUnit);
        AssetManager::getTexture(material.albedo).bind(initialUnit);
    }

    if (material.normal) {
        shader->setInt("normalMap", initialUnit+1);
        AssetManager::getTexture(material.normal).bind(initialUnit + 1);
    }

    if (material.height) {
        shader->setFloat("height_scale", 0.1);
        shader->setInt("heightMap", initialUnit+2);
        AssetManager::getTexture(material.height).bind(initialUnit + 2);
    }

    if (material.metallic) {
        shader->setInt("metallicMap", initialUnit+3);
        AssetManager::getTexture(material.metallic).bind(initialUnit + 3);
    }

    if (material.roughness) {
        shader->setInt("roughnessMap", initialUnit+4);
        AssetManager::getTexture(material.roughness).bind(initialUnit + 4);
    }

    if (material.specular) {
        shader->setInt("specularMap", initialUnit+5);
        AssetManager::getTexture(material.specular).bind(initialUnit + 5);
    }

    mesh.draw();
}

void RenderSystem::drawGbuffer(
    const MeshComponent&       mesh,
    const TransformComponent&  transform,
    const MaterialComponent&   material,
    const PointLightComponent& pointlight
) {
    const Shader* shader = material.gbufferShader;

    shader->use();
    bindCameraGlobals(shader);

    shader->setMat4("model",      transform.model);
    shader->setVec3("lightColor", pointlight.color);

    mesh.draw();
}

void RenderSystem::drawGbuffer(
    const InstanceComponent& instance,
    const MaterialComponent& material
) {
    const Shader* shader = material.gbufferShader;

    shader->use();
    bindCameraGlobals(shader);

    const uint32_t initialUnit = 0;

    if (material.albedo) {
        material.shader->setInt("albedo", initialUnit);
        AssetManager::getTexture(material.albedo).bind(initialUnit);
    }

    if (material.normal) {
        material.shader->setInt("normalMap", initialUnit+1);
        AssetManager::getTexture(material.normal).bind(initialUnit + 1);
    }

    instance.draw();
}

// ------------------------------------------------------------------------------------------------------- //

void RenderSystem::renderGbuffer(const ECS& ecs) {

    const EntityManager&    entityManager    = ecs.entityManager;
    const ComponentManager& componentManager = ecs.componentManager;

    const std::vector<Entity> objects   = ecs.view<MeshComponent, TransformComponent, MaterialComponent>();
    const std::vector<Entity> instances = ecs.view<InstanceComponent, MaterialComponent>();
    const std::vector<Entity> models    = ecs.view<ModelComponent, TransformComponent, MaterialComponent>();

    const std::vector<Entity> objects_with_LOD = ecs.view<MeshLODComponent, TransformComponent, MaterialComponent>();
    const std::vector<Entity> models_with_LOD  = ecs.view<ModelLODComponent, TransformComponent>();

    // Frustum
    Frustum frustum(Camera::get_projection() * Camera::get_view());

    // NOTE: FIX THIS - ALSO RENDERING LIGHT SOURCES (WHICH THEN UNDERGOES TONE-MAPPING)
    for (const Entity& entity : objects) {

        if (componentManager.has<PointLightComponent>(entity)) continue;

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const MeshComponent&      mesh      = componentManager.get<MeshComponent>(entity);
        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
        const MaterialComponent&  material  = componentManager.get<MaterialComponent>(entity);
        
        if (!transform.isVisible) continue;
        if (material.gbufferShader) drawGbuffer(mesh, transform, material);
    }

    for (const Entity& entity : instances) {

        const InstanceComponent& instance = componentManager.get<InstanceComponent>(entity);
        const MaterialComponent& material = componentManager.get<MaterialComponent>(entity);

        if (material.gbufferShader) drawGbuffer(instance, material);
    }

    for (const Entity& entity : models) {

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const ModelComponent&     model     = componentManager.get<ModelComponent>(entity);
        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);

        if (!transform.isVisible) continue;
        const uint total_meshes = componentManager.get<ModelComponent>(entity).meshes.size();

        MaterialComponent material = componentManager.get<MaterialComponent>(entity);

        for (uint i = 0 ; i < total_meshes; i++) {

            const MeshComponent& mesh = model.meshes[i];
            material.albedo = model.textures[i];

            if (material.gbufferShader) drawGbuffer(mesh, transform, material);
        }
    }

    // LOD System
    for (const Entity& entity : objects_with_LOD) {

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
        const MaterialComponent&  material  = componentManager.get<MaterialComponent>(entity);
        
        if (!transform.isVisible) continue;

        float distance = glm::length(Camera::instance().getPos() - transform.position);
        const MeshComponent& mesh = componentManager.get<MeshLODComponent>(entity).getMesh(distance);

        if (material.gbufferShader) drawGbuffer(mesh, transform, material);
    }

    for (const Entity& entity : models_with_LOD) {

        // Frustum Culling
        if (componentManager.has<BoundingAABBComponent>(entity)) {

            const BoundingAABBComponent& boundingAABB = componentManager.get<BoundingAABBComponent>(entity);
            if (!frustum.isMeshInside(boundingAABB)) continue;
        }

        const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
        MaterialComponent material = componentManager.get<MaterialComponent>(entity);
        
        if (!transform.isVisible) continue;

        float distance = glm::length(Camera::instance().getPos() - transform.position);
        const ModelComponent& model = componentManager.get<ModelLODComponent>(entity).getMesh(distance);

        const uint total_meshes = model.meshes.size();

        for (uint i = 0 ; i < total_meshes; i++) {

            const MeshComponent& mesh = model.meshes[i];
            material.albedo = model.textures[i];

            if (material.gbufferShader) drawGbuffer(mesh, transform, material);
        }
    }
}

void RenderSystem::lightningPass(const ECS& ecs, const Shader* shader) {

    const EntityManager&    entityManager    = ecs.entityManager;
    const ComponentManager& componentManager = ecs.componentManager;

    lastTextureUnit = 5;
    const uint32_t light_count = entityManager.emissiveEntities.size();

    shader->use();

    // ----------------------- SpotLight ----------------------- //

    shader->setBool("useSpotLight", DefaultLights::instance().flashlight.isVisible);
    shader->setSpotLight("sl", DefaultLights::instance().flashlight);

    // ------------------------ Camera ------------------------- //
    
    bindCameraGlobals(shader);

    // ----------------------- PointLight ---------------------- //

    shader->setInt("light_count", light_count);
    shader->setFloat("far_plane", 25.0f);

    for (uint32_t index = 0; index < light_count; index++) {

        const Entity& entity = entityManager.emissiveEntities[index];

        if (
            componentManager.has<PointLightComponent>(entity) &&
            componentManager.has<TransformComponent>(entity)
        ) {
            const PointLightComponent& pointlight  = componentManager.get<PointLightComponent>(entity);
            const TransformComponent&  transform   = componentManager.get<TransformComponent>(entity);

            const std::string e  = "pl[" + std::to_string(index) + "]";

            shader->setVec3((e + ".position").c_str(), transform.position);
            shader->setVec3((e + ".color").c_str(),    pointlight.color);

            shader->setFloat((e + ".constant").c_str(),  pointlight.constant);
            shader->setFloat((e + ".linear").c_str(),    pointlight.linear);
            shader->setFloat((e + ".quadratic").c_str(), pointlight.quadratic);
        }
    }

    // ------------------------ DepthMap ----------------------- //

    for (uint8_t i = 0; i < MAX_LIGHTS; i++) {

        if (i < light_count) componentManager.pointShadowFrames[i].frame->bindTexture(lastTextureUnit);
        shader->setInt(("depthMap[" + std::to_string(i) + "]").c_str(), lastTextureUnit++);
    }

    // --------------------- Directional Light ----------------- //

    shader->setDirectionalLight("dl", DefaultLights::instance().sunlight);
    shader->setBool("useDirectionalLight", Input::isKeyPressed(GLFW_KEY_L));
    // shader->setBool("useDirectionalLight", true);

    shader->setInt("dl_depthMap", lastTextureUnit);
    componentManager.directShadowFrames[0].frame->bindTexture(lastTextureUnit++);

    shader->setMat4("lightSpaceMatrix", componentManager.directShadowFrames[0].lightSpaceMatrix);
    
    // ---------------------- Environment ---------------------- //

    shader->setInt("irradianceMap", lastTextureUnit);
    entityManager.env->bindIrradiance(lastTextureUnit++);

    shader->setBool("useIrradiance", Input::isKeyPressed(GLFW_KEY_B));
    // shader->setBool("useIrradiance", true);

    shader->setInt("preFilterMap", lastTextureUnit);
    // entityManager.env->bindPrefilter(lastTextureUnit++);
    entityManager.env->bindTexture(lastTextureUnit++);

    shader->setInt("brdfLUT", lastTextureUnit);
    entityManager.env->bindBRDF(lastTextureUnit++);
}

// ------------------------------------------------------------------------------------------------------- //

bool ShadowSystem::init() {

    const float near = 1.0f;
    const float far  = 25.0f;

    shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);

    return true;
}

void ShadowSystem::render(const ECS& ecs) {

    const EntityManager&    entityManager    = ecs.entityManager;
    const ComponentManager& componentManager = ecs.componentManager;

    Shader shader[2] = {
        *Shaders::getPointLightShadow(),
        *Shaders::getPointLightShadow_Instanced()
    };

    for (const PointShadowData& pointShadow : componentManager.pointShadowFrames) {

        // Preparing each Frame of each PointLight Entity
        const glm::vec3& lightPos = componentManager.get<TransformComponent>(pointShadow.entity).position;
        
        const std::vector <glm::mat4> shadowMatrices = {

            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f,0.0f), glm::vec3(0.0f,0.0f, 1.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f,0.0f, 1.0f), glm::vec3(0.0f,-1.0f,0.0f)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,-1.0f,0.0f))
        };
        
        pointShadow.frame->bindFBO();
        glViewport(0, 0, frameBuffers::shadowSize, frameBuffers::shadowSize);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (uint8_t i = 0; i < 2; i++) {

            shader[i].use();
            shader[i].setVec3 ("lightPos" , lightPos);
            shader[i].setFloat("far_plane", 25.0f);
            
            for (uint8_t j = 0; j < 6; j++) {
                shader[i].setMat4(("shadowMatrices[" + std::to_string(j) + "]").c_str(), shadowMatrices[j]);
            }
        }

        // Rendering meshes of VisibleEntities
        const std::vector<Entity> objects   = ecs.view<MeshComponent, TransformComponent, MaterialComponent>();
        const std::vector<Entity> instances = ecs.view<InstanceComponent, MaterialComponent>();
        const std::vector<Entity> models    = ecs.view<ModelComponent, TransformComponent>();

        const std::vector<Entity> objects_with_LOD = ecs.view<MeshLODComponent, TransformComponent, MaterialComponent>();
        const std::vector<Entity> models_with_LOD  = ecs.view<ModelLODComponent, TransformComponent, MaterialComponent>();

        // NOTE: FIX THIS - ALSO RENDERING LIGHT SOURCES (WHICH THEN UNDERGOES TONE-MAPPING)
        for (const Entity& entity : objects) {

            const MeshComponent&      mesh      = componentManager.get<MeshComponent>(entity);
            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);

            drawShadow(shader[0], mesh, transform);
        }

        for (const Entity& entity : instances) {

            const InstanceComponent& instance = componentManager.get<InstanceComponent>(entity);
            drawShadowInstanced(shader[1], instance);
        }

        for (const Entity& entity : models) {

            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
            const uint total_meshes = componentManager.get<ModelComponent>(entity).meshes.size();

            for (uint i = 0 ; i < total_meshes; i++) {

                const MeshComponent& mesh = componentManager.get<ModelComponent>(entity).meshes[i];
                drawShadow(shader[0], mesh, transform);
            }
        }

        // LOD System
        for (const Entity& entity : objects_with_LOD) {

            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
            
            float distance = glm::length(Camera::instance().getPos() - transform.position);
            const MeshComponent& mesh = componentManager.get<MeshLODComponent>(entity).getMesh(distance);

            drawShadow(shader[0], mesh, transform);
        }

        for (const Entity& entity : models_with_LOD) {

            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
            
            float distance = glm::length(Camera::instance().getPos() - transform.position);
            const ModelComponent& model = componentManager.get<ModelLODComponent>(entity).getMesh(distance);

            const uint total_meshes = model.meshes.size();

            for (uint i = 0 ; i < total_meshes; i++) {

                const MeshComponent& mesh = model.meshes[i];
                drawShadow(shader[0], mesh, transform);
            }
        }
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowSystem::renderDirectional(const ECS& ecs) {

    const EntityManager&    entityManager    = ecs.entityManager;
    const ComponentManager& componentManager = ecs.componentManager;

    Renderer::disableCulling();

    for (const DirectShadowData& directShadow : componentManager.directShadowFrames) {

        const Shader shader[2] = {
            *Shaders::getDirectLightShadow(),
            *Shaders::getDirectLightShadow_Instanced()
        };

        for (uint8_t i = 0; i < 2; i++) {

            shader[i].use();
            shader[i].setMat4("lightSpace", directShadow.lightSpaceMatrix);
        }

        directShadow.frame->bindFBO();
        glViewport(0,0, frameBuffers::shadowSize, frameBuffers::shadowSize);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // Rendering meshes of VisibleEntities
        const std::vector<Entity> objects   = ecs.view<MeshComponent, TransformComponent, MaterialComponent>();
        const std::vector<Entity> instances = ecs.view<InstanceComponent, MaterialComponent>();
        const std::vector<Entity> models    = ecs.view<ModelComponent, TransformComponent>();

        const std::vector<Entity> objects_with_LOD = ecs.view<MeshLODComponent, TransformComponent, MaterialComponent>();
        const std::vector<Entity> models_with_LOD  = ecs.view<ModelLODComponent, TransformComponent, MaterialComponent>();

        // NOTE: FIX THIS - ALSO RENDERING LIGHT SOURCES (WHICH THEN UNDERGOES TONE-MAPPING)
        for (const Entity& entity : objects) {

            const MeshComponent&      mesh      = componentManager.get<MeshComponent>(entity);
            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);

            drawShadow(shader[0], mesh, transform);
        }

        for (const Entity& entity : instances) {

            const InstanceComponent& instance = componentManager.get<InstanceComponent>(entity);
            drawShadowInstanced(shader[1], instance);
        }

        for (const Entity& entity : models) {

            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
            const uint total_meshes = componentManager.get<ModelComponent>(entity).meshes.size();

            for (uint i = 0 ; i < total_meshes; i++) {

                const MeshComponent& mesh = componentManager.get<ModelComponent>(entity).meshes[i];
                drawShadow(shader[0], mesh, transform);
            }
        }

        // LOD System
        for (const Entity& entity : objects_with_LOD) {

            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
            
            float distance = glm::length(Camera::instance().getPos() - transform.position);
            const MeshComponent& mesh = componentManager.get<MeshLODComponent>(entity).getMesh(distance);

            drawShadow(shader[0], mesh, transform);
        }

        for (const Entity& entity : models_with_LOD) {

            const TransformComponent& transform = componentManager.get<TransformComponent>(entity);
            
            float distance = glm::length(Camera::instance().getPos() - transform.position);
            const ModelComponent& model = componentManager.get<ModelLODComponent>(entity).getMesh(distance);

            const uint total_meshes = model.meshes.size();

            for (uint i = 0 ; i < total_meshes; i++) {

                const MeshComponent& mesh = model.meshes[i];
                drawShadow(shader[0], mesh, transform);
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowSystem::drawShadow(
    const Shader&             shader,
    const MeshComponent&      mesh,
    const TransformComponent& transform
) {
    shader.use();
    shader.setMat4("model", transform.model);
    mesh.draw();
}

void ShadowSystem::drawShadowInstanced(
    const Shader&            shader,
    const InstanceComponent& instance
) {
    shader.use();
    instance.draw();
}
