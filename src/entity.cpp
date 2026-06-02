// #include <entity.h>

// #include <camera.h>
// #include <shader.h>
// #include <shapes.h>
// #include <renderer.h>

// #include <s_time.h>

// ECS::ECS():
//     hasPointLight(MAX_ENTITIES, false)
// {
//     transforms.reserve(MAX_ENTITIES);
//     meshes.reserve(MAX_ENTITIES);
//     materials.reserve(MAX_ENTITIES);

//     lightSources.reserve(MAX_ENTITIES);
// }

// // ------------------------------ ECS Renderer --------------------------------------- //

// void RenderSystem::bindCameraGlobals(const Shader* shader) const {

//     shader->setMat4("projection", Camera::instance().getPerspective());
//     shader->setMat4("view",       Camera::instance().getView());
//     shader->setVec3("camPos",     Camera::instance().getPos());
// }

// void RenderSystem::bindPointLightGlobals(const ECS& ecs) const {

//     // running::time::startInterval();

//     for (Entity entity = 0; entity < ecs.getTotal(); entity++) {

//         if (ecs.hasPointLight[entity]) {
            
//             const Shader* shader = ecs.materials[entity].shader;

//             shader->use();

//             shader->setInt("light_count", ecs.total_lights);
//             shader->setFloat("far_plane", 25.0f);

//             for (uint32_t pl = 0; pl < ecs.total_lights; pl++) {
//                 shader->setPointLight(pl, ecs.lightSources[entity]);
//             }
//         }
//     } 

//     // const uint64_t time = running::time::endInterval();
//     // std::cout << "bindPointLightGlobals: " << time << running::time::unit << std::endl;
// }

// void RenderSystem::render(const ECS& ecs) const {

//     bindPointLightGlobals(ecs);

//     for (Entity entity = 0; entity < ecs.getTotal(); entity++) {

//         const Transform&     transform = ecs.transforms[entity];
//         const EntityMesh&    mesh      = ecs.meshes[entity];
//         const MaterialPhong& material  = ecs.materials[entity];

//         draw(mesh, transform, material);
//     }
// }

// void RenderSystem::draw(const EntityMesh& mesh, const Transform& transform, const MaterialPhong& material) const {

//     material.shader->use();

//     material.shader->setMat4("model",        transform.model);
//     material.shader->setMat4("normalMatrix", transform.normalMatrix);

//     bindCameraGlobals(material.shader);

//     if (material.albedo) {
//         material.shader->setInt("albedo", 0);
//         material.albedo->bind(0);
//     }

//     mesh.shape->draw();
// }

// // ------------------------------ ECS Renderer --------------------------------------- //

// void Transform::computeModel() {

//     model = glm::translate(glm::mat4(1.0f), position);

//     model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
//     model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
//     model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

//     model = glm::scale(model, scale);

//     normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
// }

// EntityShapes::EntityShapes() {

//     cube   = ecs.createEntity();
//     square = ecs.createEntity();
    
//     // cube
//     ecs.meshes[cube].shape     = &DefaultShapes::instance().cube;
//     ecs.materials[cube].shader = Shaders::get(ALBEDO3D);
//     ecs.materials[cube].albedo = DefaultShapes::instance().cube.getAlbedo();
    
//     {
//         Transform transform {};

//         transform.position = glm::vec3(-3.0f, 0.0f, 3.0f);
//         transform.rotation = glm::vec3(0.0f);
//         transform.scale    = glm::vec3(1.0f);

//         transform.computeModel();
//         ecs.transforms[cube] = transform;
//     }

//     // square
//     ecs.meshes[square].shape     = &DefaultShapes::instance().square;
//     ecs.materials[square].shader = Shaders::get(ALBEDO2D);
//     ecs.materials[square].albedo = DefaultShapes::instance().square.getAlbedo();

//     {
//         Transform transform {};

//         transform.position = glm::vec3(-3.0f, 0.0f, 5.0f);
//         transform.rotation = glm::vec3(0.0f);
//         transform.scale    = glm::vec3(1.0f);

//         transform.computeModel();
//         ecs.transforms[square] = transform;
//     }
// }