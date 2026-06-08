#include <ecs/rendersystem.h>

#include <camera.h>
#include <shader.h>
#include <shapes.h>
#include <renderer.h>
#include <debug.h>
#include <lights.h>
#include <frame.h>
#include <input.h>

#include <iostream>

uint8_t RenderSystem::lastTextureUnit = 0;

void RenderSystem::bindCameraGlobals(const Shader* shader) {

    shader->setMat4("projection", Camera::instance().getPerspective());
    shader->setMat4("view",       Camera::instance().getView());
    shader->setVec3("camPos",     Camera::instance().getPos());
}

void RenderSystem::bindPointLightGlobals(const EntityManager& entityManager, const ComponentManager& componentManager) {

    lastTextureUnit = 0;
    const uint32_t light_count = entityManager.emissiveEntities.size();

    for (const Shader* shader : componentManager.uniqueShaders) {

        shader->use();

        // ----------------------- SpotLight ----------------------- //

        shader->setBool("useSpotLight", DefaultLights::instance().flashlight.isVisible);
        shader->setSpotLight("sl", DefaultLights::instance().flashlight);

        // ------------------------ Camera ------------------------- //
        
        bindCameraGlobals(shader);

        // ----------------------- PointLight ---------------------- //

        shader->setInt("light_count", light_count);
        shader->setFloat("far_plane", 25.0f);

        for (uint32_t entity = 0; entity < light_count; entity++) {

            const Entity& light = entityManager.emissiveEntities[entity];
            
            const PointLightComponent& pointlight  = componentManager.arr_light[light];
            const TransformComponent&  transform   = componentManager.arr_transform[light];

            const std::string e  = "pl[" + std::to_string(entity) + "]";

            shader->setVec3((e + ".position").c_str(), transform.position);
            shader->setVec3((e + ".color").c_str(),    pointlight.color);

            shader->setFloat((e + ".constant").c_str(),  pointlight.constant);
            shader->setFloat((e + ".linear").c_str(),    pointlight.linear);
            shader->setFloat((e + ".quadratic").c_str(), pointlight.quadratic);
        }

        // ------------------------ DepthMap ----------------------- //

        for (const PointShadowData& pointShadow : componentManager.pointShadowFrames) {

            shader->setInt(("depthMap[" + std::to_string(lastTextureUnit) + "]").c_str() ,lastTextureUnit);
            pointShadow.frame->bindTexture(lastTextureUnit++);
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

void RenderSystem::render(const EntityManager& entityManager, const ComponentManager& componentManager) {

    bindPointLightGlobals(entityManager, componentManager);

    for (const Entity& entity : entityManager.visibleEntities) {

        if (
            componentManager.has_mesh[entity] &&
            componentManager.has_transform[entity] &&
            componentManager.has_material[entity]
        ) {
            const MeshComponent&      mesh      = componentManager.arr_mesh[entity];
            const TransformComponent& transform = componentManager.arr_transform[entity];
            const MaterialComponent&  material  = componentManager.arr_material[entity];
    
            draw(mesh, transform, material);
        }

        if (
            componentManager.has_model[entity] &&
            componentManager.has_transform[entity]
        ) {
            
            const TransformComponent& transform = componentManager.arr_transform[entity];
            const uint total_meshes = componentManager.arr_model[entity].meshes.size();

            for (uint i = 0 ; i < total_meshes; i++) {

                const MeshComponent&     mesh     = componentManager.arr_model[entity].meshes[i];
                const MaterialComponent& material = componentManager.arr_model[entity].materials[i];

                draw(mesh, transform, material);
            }
        }

        if (
            componentManager.has_instance[entity] && 
            componentManager.has_material[entity]
        ) {
            const InstanceComponent& instance = componentManager.arr_instance[entity];
            const MaterialComponent& material = componentManager.arr_material[entity];

            draw(instance, material);
        }
    }
}

void RenderSystem::renderLights(const EntityManager& entityManager, const ComponentManager& componentManager) {

    for (const Entity& entity : entityManager.emissiveEntities) {

        if (componentManager.has_light[entity]) {

            const MeshComponent&       mesh       = componentManager.arr_mesh[entity];
            const TransformComponent&  transform  = componentManager.arr_transform[entity];
            const MaterialComponent&   material   = componentManager.arr_material[entity];
            const PointLightComponent& pointlight = componentManager.arr_light[entity];
            
            draw(mesh, transform, material, pointlight);
        }
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
        material.albedo->bind(initialUnit);
    }

    if (material.normal) {
        shader->setInt("normalMap", initialUnit+1);
        material.normal->bind(initialUnit+1);
    }

    if (material.height) {
        shader->setFloat("height_scale", 0.1);
        shader->setInt("heightMap", initialUnit+2);
        material.height->bind(initialUnit+2);
    }

    if (material.metallic) {
        shader->setInt("metallicMap", initialUnit+3);
        material.metallic->bind(initialUnit+3);
    }

    if (material.roughness) {
        shader->setInt("roughnessMap", initialUnit+4);
        material.roughness->bind(initialUnit+4);
    }

    mesh.shape.draw();
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

    mesh.shape.draw();
}

void RenderSystem::draw(
    const InstanceComponent& instance,
    const MaterialComponent& material
) {

    material.shader->use();

    const uint32_t initialUnit = lastTextureUnit;

    if (material.albedo) {
        material.shader->setInt("albedo", initialUnit);
        material.albedo->bind(initialUnit);
    }

    if (material.normal) {
        material.shader->setInt("normalMap", initialUnit+1);
        material.albedo->bind(initialUnit+1);
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
        material.albedo->bind(initialUnit);
    }

    if (material.normal) {
        shader->setInt("normalMap", initialUnit+1);
        material.normal->bind(initialUnit+1);
    }

    if (material.height) {
        shader->setFloat("height_scale", 0.1);
        shader->setInt("heightMap", initialUnit+2);
        material.height->bind(initialUnit+2);
    }

    if (material.metallic) {
        shader->setInt("metallicMap", initialUnit+3);
        material.metallic->bind(initialUnit+3);
    }

    if (material.roughness) {
        shader->setInt("roughnessMap", initialUnit+4);
        material.roughness->bind(initialUnit+4);
    }

    mesh.shape.draw();
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

    mesh.shape.draw();
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
        shader->setInt("albedo", initialUnit);
        material.albedo->bind(initialUnit);
    }

    if (material.normal) {
        shader->setInt("normalMap", initialUnit+1);
        material.albedo->bind(initialUnit+1);
    }

    instance.draw();
}

// ------------------------------------------------------------------------------------------------------- //

void RenderSystem::renderGbuffer(const EntityManager& entityManager, const ComponentManager& componentManager) {

    for (const Entity& entity : entityManager.visibleEntities) {

        if (
            componentManager.has_mesh[entity] &&
            componentManager.has_transform[entity] &&
            componentManager.has_material[entity]
        ) {
            const MeshComponent&      mesh      = componentManager.arr_mesh[entity];
            const TransformComponent& transform = componentManager.arr_transform[entity];
            const MaterialComponent&  material  = componentManager.arr_material[entity];
    
            if (material.gbufferShader) drawGbuffer(mesh, transform, material);
        }

        if (
            componentManager.has_model[entity] &&
            componentManager.has_transform[entity]
        ) {
            
            const TransformComponent& transform = componentManager.arr_transform[entity];
            const uint total_meshes = componentManager.arr_model[entity].meshes.size();

            for (uint i = 0 ; i < total_meshes; i++) {

                const MeshComponent&     mesh     = componentManager.arr_model[entity].meshes[i];
                const MaterialComponent& material = componentManager.arr_model[entity].materials[i];

                if (material.gbufferShader) drawGbuffer(mesh, transform, material);
            }
        }

        if (
            componentManager.has_instance[entity] && 
            componentManager.has_material[entity]
        ) {
            const InstanceComponent& instance = componentManager.arr_instance[entity];
            const MaterialComponent& material = componentManager.arr_material[entity];

            if (material.gbufferShader) drawGbuffer(instance, material);
        }
    }
}

void RenderSystem::lightningPass(const EntityManager& entityManager, const ComponentManager& componentManager, const Shader* shader) {

    lastTextureUnit = 4;
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

    for (uint32_t entity = 0; entity < light_count; entity++) {

        const Entity& light = entityManager.emissiveEntities[entity];
        
        const PointLightComponent& pointlight  = componentManager.arr_light[light];
        const TransformComponent&  transform   = componentManager.arr_transform[light];

        const std::string e  = "pl[" + std::to_string(entity) + "]";

        shader->setVec3((e + ".position").c_str(), transform.position);
        shader->setVec3((e + ".color").c_str(),    pointlight.color);

        shader->setFloat((e + ".constant").c_str(),  pointlight.constant);
        shader->setFloat((e + ".linear").c_str(),    pointlight.linear);
        shader->setFloat((e + ".quadratic").c_str(), pointlight.quadratic);
    }

    // ------------------------ DepthMap ----------------------- //

    int index = 0;
    for (const PointShadowData& pointShadow : componentManager.pointShadowFrames) {

        shader->setInt(("depthMap[" + std::to_string(index++) + "]").c_str() ,lastTextureUnit);
        pointShadow.frame->bindTexture(lastTextureUnit++);
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
    
    // DebugMenu::log((unsigned int)lastTextureUnit);
}

// ------------------------------------------------------------------------------------------------------- //

glm::mat4 ShadowSystem::shadowProj = glm::mat4(1.0f);

bool ShadowSystem::init() {

    const float near = 1.0f;
    const float far  = 25.0f;

    shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);

    return true;
}

void ShadowSystem::render(const EntityManager& entityManager, const ComponentManager& componentManager) {

    Shader shader[2] = {
        *Shaders::getPointLightShadow(),
        *Shaders::getPointLightShadow_Instanced()
    };

    for (const PointShadowData& pointShadow : componentManager.pointShadowFrames) {

        // Preparing each Frame of each PointLight Entity
        const glm::vec3& lightPos = componentManager.arr_transform[pointShadow.entity].position;
        
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
        for (const Entity& entity : entityManager.visibleEntities) {
            
            if (
                componentManager.has_mesh[entity] &&
                componentManager.has_transform[entity]
            ) {
                const MeshComponent&      mesh      = componentManager.arr_mesh[entity];
                const TransformComponent& transform = componentManager.arr_transform[entity];
                
                drawShadow(shader[0], mesh, transform);
            }

            if (
                componentManager.has_instance[entity]
            ) {
                const InstanceComponent& instance = componentManager.arr_instance[entity];

                drawShadowInstanced(shader[1], instance);
            }
        }
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowSystem::renderDirectional(const EntityManager& entityManager, const ComponentManager& componentManager) {

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
        for (const Entity& entity : entityManager.visibleEntities) {
            
            if (
                componentManager.has_mesh[entity] &&
                componentManager.has_transform[entity]
            ) {
                const MeshComponent&      mesh      = componentManager.arr_mesh[entity];
                const TransformComponent& transform = componentManager.arr_transform[entity];
                
                drawShadow(shader[0], mesh, transform);
            }

            if (
                componentManager.has_instance[entity]
            ) {
                const InstanceComponent& instance = componentManager.arr_instance[entity];

                drawShadowInstanced(shader[1], instance);
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
    mesh.shape.draw();
}

void ShadowSystem::drawShadowInstanced(
    const Shader&            shader,
    const InstanceComponent& instance
) {
    shader.use();
    instance.draw();
}