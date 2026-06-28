#include <scenes.h>
#include <camera.h>
#include <renderer.h>
#include <debug.h>
#include <input.h>
#include <systems/update.h>
#include <assetManager.h>

#include <glm/gtx/rotate_vector.hpp>

void Scene2::init() {

    EntityManager&    entityManager    = ecs.entityManager;
    ComponentManager& componentManager = ecs.componentManager;

    running::globalTimer::startInterval();

    wood_box  = entityManager.createEntity();
    floor     = entityManager.createEntity();
    wall      = entityManager.createEntity();
    light1    = entityManager.createEntity();
    light2    = entityManager.createEntity();
    cubes     = entityManager.createEntity();
    sphere    = entityManager.createEntity();
    brickWall = entityManager.createEntity();
    headcam   = entityManager.createEntity();
    sun       = entityManager.createEntity();
    gun       = entityManager.createEntity();

    mainCamera = entityManager.createEntity();

    // entityManager.visibleEntities.push_back(wood_box);
    // entityManager.visibleEntities.push_back(floor);
    // entityManager.visibleEntities.push_back(wall);
    // entityManager.visibleEntities.push_back(cubes);
    // entityManager.visibleEntities.push_back(sphere);
    // entityManager.visibleEntities.push_back(brickWall);
    // entityManager.visibleEntities.push_back(headcam);

    entityManager.emissiveEntities.push_back(light1);
    entityManager.emissiveEntities.push_back(light2);

    // Directional Light
    float near_plane = 1.0f, far_plane = 10.0f, size = 10.0f;
    glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, near_plane, far_plane);

    glm::vec3 dirLight_src = glm::vec3(5.0f, 4.0f, 0.0f);

    glm::mat4 lightView = glm::lookAt(
        dirLight_src,
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    DirectShadowData sunlight(
        sun,
        Matrix4(lightProjection * lightView),
        new DirectShadowFrame()
    );

    componentManager.directShadowFrames.push_back(sunlight);

    // Environment Map
    // std::vector <std::string> envFaces = {
        
    //     "assets/env/right.png",
    //     "assets/env/left.png",
    //     "assets/env/top.png",
    //     "assets/env/bottom.png",
    //     "assets/env/front.png",
    //     "assets/env/back.png"
    // };

    // env = new Environment(envFaces);
    env = new Environment("assets/env/hdri-sky.hdr", 1024);
    entityManager.env = env;

    DebugMenu::log("HDR Environment: " + running::globalTimer::endInterval());

    // mainCamera
    {
        CameraComponent cameraComponent;
        cameraComponent.camera = Camera::instance();
        
        TransformComponent transform;
        transform.position = cameraComponent.camera.getPos();
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(1.0f);

        componentManager.addComponent(mainCamera, transform);
        componentManager.addComponent(mainCamera, cameraComponent);

        Camera::activeCamera = &componentManager.get<CameraComponent>(mainCamera).camera;
    }

    // wood_box
    {
        TransformComponent transform;
        transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(1.0f);
        transform.computeModel();

        // transform.isVisible = false;
        
        MeshComponent mesh = EntityShapes::instance().cubeNorm;
        
        DebugMenu::log("EntityShapes (init): " + running::globalTimer::endInterval());
        
        MaterialComponent material;
        material.shader = Shaders::get(NORMPBR3D);
        
        material.albedo = AssetManager::loadTexture("assets/textures/wood_box.png");
        // material.albedo = AssetManager::loadTexture_flatColor(glm::vec4(colors::WHITE, 1.0f));
        material.normal = AssetManager::loadTexture("assets/textures/wood_box_normal.png", 1);
        
        material.gbufferShader = Shaders::get(GBUFFERNORM_3D);
        
        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = transform.position;
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        BoundingAABBComponent AABB = createAABB(transform, mesh);
        
        componentManager.addComponent(wood_box, mesh);
        componentManager.addComponent(wood_box, transform);
        componentManager.addComponent(wood_box, material);
        componentManager.addComponent(wood_box, boundingSphere);
        componentManager.addComponent(wood_box, AABB);
    }

    DebugMenu::log("WoodBox: " + running::globalTimer::endInterval());

    // for (uint32_t i = 0; i < 100; i++) {

    //     crowd[i] = entityManager.createEntity();

    //     TransformComponent transform;
    //     transform.position = randomPosition(-20.0f, 20.0f);
    //     transform.rotation = glm::vec3(0.0f);
    //     transform.scale    = glm::vec3(1.0f);
    //     transform.computeModel();
        
    //     MeshComponent mesh;
    //     mesh = EntityShapes::instance().cubeNorm;
        
    //     MaterialComponent material;
    //     material.shader = Shaders::get(NORMPBR3D);
        
    //     material.albedo = componentManager.get<MaterialComponent>(wood_box).albedo;
    //     material.normal = componentManager.get<MaterialComponent>(wood_box).normal;
        
    //     material.gbufferShader = Shaders::get(GBUFFERNORM_3D);
        
    //     material.metallic  = material.albedo;
    //     material.roughness = material.albedo;

    //     BoundingSphereComponent boundingSphere;
    //     boundingSphere.center = transform.position;
    //     boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

    //     BoundingAABBComponent AABB {
    //         transform.position,
    //         mesh.minCorner,
    //         mesh.maxCorner
    //     };

    //     AABB.recompute(transform.model);
        
    //     componentManager.addComponent(crowd[i], mesh);
    //     componentManager.addComponent(crowd[i], transform);
    //     componentManager.addComponent(crowd[i], material);
    //     componentManager.addComponent(crowd[i], boundingSphere);
    //     componentManager.addComponent(crowd[i], AABB);
    // }

    // DebugMenu::log("crowd: " + running::globalTimer::endInterval());

    // floor
    {
        TransformComponent transform;
        transform.position = glm::vec3(0.0f, -2.0f, 0.0f);
        transform.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
        transform.scale    = glm::vec3(10.0f);
        transform.computeModel();

        MeshComponent mesh = EntityShapes::instance().square;

        MaterialComponent material;
        material.shader = Shaders::get(NORMPBR2D);

        material.albedo   = AssetManager::loadTexture("assets/textures/brickwall.jpg");
        material.normal   = AssetManager::loadTexture("assets/textures/brickwall_normal.png", 1);
        material.specular = AssetManager::loadTexture("assets/textures/stone_floor.jpg");

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        material.gbufferShader = Shaders::get(GBUFFERNORM_2D);

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        BoundingAABBComponent AABB = createAABB(transform, mesh);
        
        componentManager.addComponent(floor, mesh);
        componentManager.addComponent(floor, transform);
        componentManager.addComponent(floor, material);
        componentManager.addComponent(floor, boundingSphere);
        componentManager.addComponent(floor, AABB);
    }

    DebugMenu::log("Floor: " + running::globalTimer::endInterval());

    // wall
    {
        TransformComponent transform;
        transform.position = glm::vec3(-2.5f, 0.5f, -5.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(5.0f);
        transform.computeModel();

        MeshComponent mesh = EntityShapes::instance().square;

        MaterialComponent material;
        material.shader   = Shaders::get(NORMPBR2D);
        material.albedo   = componentManager.get<MaterialComponent>(floor).albedo;
        material.normal   = componentManager.get<MaterialComponent>(floor).normal;
        material.specular = componentManager.get<MaterialComponent>(floor).specular;

        material.gbufferShader = Shaders::get(GBUFFERNORM_2D);

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        BoundingAABBComponent AABB = createAABB(transform, mesh);
        
        componentManager.addComponent(wall, mesh);
        componentManager.addComponent(wall, transform);
        componentManager.addComponent(wall, material);
        componentManager.addComponent(wall, boundingSphere);
        componentManager.addComponent(wall, AABB);
    }

    DebugMenu::log("Wall: " + running::globalTimer::endInterval());

    // Parallax wall
    {
        TransformComponent transform;
        transform.position = glm::vec3(2.5f, 0.5f, -5.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(2.0f);
        transform.computeModel();

        MeshComponent mesh = EntityShapes::instance().square;

        MaterialComponent material;
        material.shader = Shaders::get(PARALLAX2D);
        // material.gbufferShader = Shaders::get(GBUFFERNORM_2D);
        
        material.albedo = AssetManager::loadTexture("assets/textures/parallax_maps/bricks2.jpg");
        material.normal = AssetManager::loadTexture("assets/textures/parallax_maps/bricks2_normal.jpg", true);
        material.height = AssetManager::loadTexture("assets/textures/parallax_maps/bricks2_disp.jpg", true);

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        BoundingAABBComponent AABB = createAABB(transform, mesh);
        
        componentManager.addComponent(brickWall, mesh);
        componentManager.addComponent(brickWall, transform);
        componentManager.addComponent(brickWall, material);
        componentManager.addComponent(brickWall, boundingSphere);
        componentManager.addComponent(brickWall, AABB);
    }

    DebugMenu::log("Parallax Wall (albedo, normal, height): " + running::globalTimer::endInterval());

    // sphere
    // {
    //     TransformComponent transform;
    //     transform.position = glm::vec3(6.0f, 0.0f, 3.0f);
    //     transform.rotation = glm::vec3(0.0f);
    //     transform.scale    = glm::vec3(1.0f);
    //     transform.computeModel();

    //     MaterialComponent material;
    //     material.shader = Shaders::get(NORMPBR3D);
    //     material.gbufferShader = Shaders::get(GBUFFER3D);

    //     material.metallic  = material.albedo;
    //     material.roughness = material.albedo;

    //     ModelComponent model0 = AssetManager::loadModel("assets/models/sphere/sphereHD.obj", true);
    //     ModelComponent model1 = AssetManager::loadModel("assets/models/sphere/sphere.obj", true);
    //     ModelComponent model2 = AssetManager::loadModel("assets/models/sphere/sphereSD.obj", true);
        
    //     ModelLODComponent meshLOD { model0, model1, model2 };

    //     BoundingAABBComponent AABB = createAABB(transform, model0);

    //     componentManager.addComponent(sphere, meshLOD);
    //     componentManager.addComponent(sphere, transform);
    //     componentManager.addComponent(sphere, material);
    //     componentManager.addComponent(sphere, AABB);
    // }

    // {
    //     TransformComponent transform;
    //     transform.position = glm::vec3(6.0f, 2.0f, 3.0f);
    //     transform.rotation = glm::vec3(0.0f);
    //     transform.scale    = glm::vec3(1.0f);
    //     transform.computeModel();

    //     MaterialComponent material;
    //     material.shader        = Shaders::get(NORMPBR3D);
    //     material.gbufferShader = Shaders::get(GBUFFER3D);

    //     material.metallic  = material.albedo;
    //     material.roughness = material.albedo;

    //     ModelComponent model = AssetManager::loadModel("assets/models/sphere/sphere_normalMapped.obj", true);

    //     BoundingAABBComponent AABB = createAABB(transform, model);

    //     componentManager.addComponent(sphere, model);
    //     componentManager.addComponent(sphere, transform);
    //     componentManager.addComponent(sphere, material);
    //     componentManager.addComponent(sphere, AABB);
    // }

    DebugMenu::log("Sphere: " + running::globalTimer::endInterval());

    // for (uint32_t i = 0; i < 100; i++) {

    //     crowd[100 + i] = entityManager.createEntity();

    //     TransformComponent transform;
    //     transform.position = randomPosition(-20.0f, 20.0f);
    //     transform.rotation = glm::vec3(0.0f);
    //     transform.scale    = glm::vec3(1.0f);
    //     transform.computeModel();

    //     MaterialComponent material = componentManager.get<MaterialComponent>(sphere);
    //     BoundingAABBComponent AABB = componentManager.get<BoundingAABBComponent>(sphere);

    //     AABB.center = transform.position;
    //     AABB.recompute(transform.model);

    //     BoundingSphereComponent boundingSphere;
    //     boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    //     boundingSphere.radius = sphereRadius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

    //     componentManager.addComponent(crowd[100 + i], componentManager.get<ModelLODComponent>(sphere));
    //     componentManager.addComponent(crowd[100 + i], transform);
    //     componentManager.addComponent(crowd[100 + i], material);
    //     componentManager.addComponent(crowd[100 + i], boundingSphere);
    //     componentManager.addComponent(crowd[100 + i], AABB);
    // }

    // headcam
    {
        TransformComponent transform;
        transform.position = glm::vec3(1.0f, 0.0f, 2.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.5f);
        transform.computeModel();

        transform.local_position = glm::vec3(1.0f, 0.0f, 0.0f);
        transform.local_rotation = glm::vec3(0.0f);
        transform.local_scale    = glm::vec3(0.5f);

        MaterialComponent material;
        material.shader = Shaders::get(PHONG3D);
        material.gbufferShader = Shaders::get(GBUFFER3D);

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        ModelComponent model = AssetManager::loadModel("assets/models/test_cube/colorCamera.obj");

        BoundingAABBComponent AABB = createAABB(transform, model);

        CameraComponent cameraComponent;
        Camera& camera = cameraComponent.camera;

        camera.set_position(transform.position);
        camera.set_target({-1.0f, 0.0f, -1.0f});

        componentManager.addComponent(headcam, model);
        componentManager.addComponent(headcam, material);
        componentManager.addComponent(headcam, transform);
        componentManager.addComponent(headcam, AABB);
        componentManager.addComponent(headcam, cameraComponent);
    }

    ChildComponent children;
    children.children = { headcam };
    componentManager.addComponent(wood_box, children);

    // cubes
    {
        MaterialComponent material;
        material.shader = Shaders::get(INSTANCEPBR3D);
        material.albedo = AssetManager::loadTexture("assets/textures/grunge-box-small.jpg");

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        material.gbufferShader = Shaders::get(INSTANCE_GBUFFER3D);

        const InstanceComponent& instance = EntityShapes::instance().cubes;
        
        componentManager.addComponent(cubes, instance);
        componentManager.addComponent(cubes, material);
    }

    DebugMenu::log("Cubes (albedo): " + running::globalTimer::endInterval());

    // light1
    {
        TransformComponent transform;
        transform.position = glm::vec3(3.0f, 2.5f, -3.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.5f);
        transform.computeModel();

        MeshComponent mesh = EntityShapes::instance().cube;

        MaterialComponent material;
        material.shader = Shaders::get(LIGHT3D);

        PointLightComponent pointlight;
        pointlight.color     = colors::YELLOW;
        pointlight.constant  = DefaultLights::instance().cubelight.constant;
        pointlight.linear    = DefaultLights::instance().cubelight.linear;
        pointlight.quadratic = DefaultLights::instance().cubelight.quadratic;

        BoundingAABBComponent AABB = createAABB(transform, mesh);

        componentManager.addComponent(light1, mesh);
        componentManager.addComponent(light1, transform);
        componentManager.addComponent(light1, material);
        // DebugMenu::log("Light1 before pl: " + running::globalTimer::endInterval());
        componentManager.addComponent(light1, pointlight);
        // DebugMenu::log("Light1 after pl: " + running::globalTimer::endInterval());
        componentManager.addComponent(light1, AABB);
    }

    DebugMenu::log("Light1: " + running::globalTimer::endInterval());

    // light2
    {
        TransformComponent transform;
        transform.position = glm::vec3(-3.0f, 2.5f, 3.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.5f);
        transform.computeModel();

        MeshComponent mesh = EntityShapes::instance().cube;

        MaterialComponent material;
        material.shader = Shaders::get(LIGHT3D);

        PointLightComponent pointlight;
        pointlight.color     = 2.0f * colors::PINK;
        pointlight.constant  = DefaultLights::instance().cubelight.constant;
        pointlight.linear    = DefaultLights::instance().cubelight.linear;
        pointlight.quadratic = DefaultLights::instance().cubelight.quadratic;

        BoundingAABBComponent AABB = createAABB(transform, mesh);

        componentManager.addComponent(light2, mesh);
        componentManager.addComponent(light2, transform);
        componentManager.addComponent(light2, material);
        // DebugMenu::log("Light2 before pl: " + running::globalTimer::endInterval());
        componentManager.addComponent(light2, pointlight);
        // DebugMenu::log("Light2 after pl: " + running::globalTimer::endInterval());
        componentManager.addComponent(light2, AABB);
    }

    // gun
    {
        TransformComponent transform;
        transform.position = glm::vec3(-1.0f, 0.0f, 1.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.1f);

        transform.local_position = glm::vec3(0.35f, -0.5f, -1.0f);
        // transform.local_rotation = glm::vec3(0.0f, 95.0f, 5.0f);
        transform.local_scale    = glm::vec3(0.2f);

        transform.computeModel();

        MaterialComponent material;
        material.shader = Shaders::get(PHONG3D);
        material.gbufferShader = Shaders::get(GBUFFER3D);

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        ModelComponent model = AssetManager::loadModel("assets/models/gun/gun.obj");

        BoundingAABBComponent AABB = createAABB(transform, model);

        componentManager.addComponent(gun, model);
        componentManager.addComponent(gun, material);
        componentManager.addComponent(gun, transform);
        componentManager.addComponent(gun, AABB);
    }

    DebugMenu::log("Gun: " + running::globalTimer::endInterval());

    ChildComponent camChild;
    camChild.children = { gun };
    componentManager.addComponent(mainCamera, camChild);

    // Multi-threaded Work
    AssetManager::compileTextures();
    DebugMenu::log("Texture Compilation: " + running::globalTimer::endInterval());
}

void Scene2::input(GLFWwindow* window, const float& delta_time) {

    static Entity  entity = 0;
    const  float   move_speed    = scene_var::speed * delta_time;

    // Model of the Object to move

    if (
        Input::isKeyDown(GLFW_KEY_KP_1) ||
        (!ecs.componentManager.has<TransformComponent>(entity))
    ) {
        entity = (entity + 1) % ecs.entityManager.total_entities();
    }

    if (ecs.componentManager.has<TransformComponent>(entity)) {

        TransformComponent& transform = ecs.componentManager.get<TransformComponent>(entity);
        Matrix4& movableModel = ecs.componentManager.get<TransformComponent>(entity).model;
        
        if (glfwGetKey(window, GLFW_KEY_KP_8)) {
            movableModel.translate(move_speed * glm::vec3( 0.0f, 0.0f,-1.0f));
        }
        
        if (glfwGetKey(window, GLFW_KEY_KP_2)) {
            movableModel.translate(move_speed * glm::vec3( 0.0f, 0.0f, 1.0f));
        }
        
        if (glfwGetKey(window, GLFW_KEY_KP_4)) {
            movableModel.translate(move_speed * glm::vec3(-1.0f, 0.0f, 0.0f));
        }
        
        if (glfwGetKey(window, GLFW_KEY_KP_6)) {
            movableModel.translate(move_speed * glm::vec3( 1.0f, 0.0f, 0.0f));
        }
        
        if (glfwGetKey(window, GLFW_KEY_KP_9)) {
            movableModel.translate(move_speed * glm::vec3( 0.0f, 1.0f, 0.0f));
        }
        
        if (glfwGetKey(window, GLFW_KEY_KP_7)) {
            movableModel.translate(move_speed * glm::vec3( 0.0f,-1.0f, 0.0f));
        }

        if (Input::isKeyPressed(GLFW_KEY_KP_3)) {
            transform.rotation.y += 100.0f * delta_time;
            transform.computeModel();
        }

        ecs.componentManager.get<TransformComponent>(entity).computePosition();
    }
    
    if (Input::isKeyPressed(GLFW_KEY_KP_5)) {
        
        const float rotation_speed = 2.0f;
        
        TransformComponent* transforms[2] = {
            &ecs.componentManager.get<TransformComponent>(light1),
            &ecs.componentManager.get<TransformComponent>(light2)
        };
        
        Matrix4 t_matrix {};
        t_matrix.rotate(rotation_speed, glm::vec3(0.0f, 1.0f, 0.0f));
        
        for (uint8_t i = 0; i < 2; i++) {
            transforms[i]->model.setMatrix(t_matrix.getMatrix() * transforms[i]->model.getMatrix());
            transforms[i]->computePosition();
        }
    }

    if (Input::isKeyDown(GLFW_KEY_T)) {
        DefaultLights::instance().flashlight.isVisible = !DefaultLights::instance().flashlight.isVisible;
    }

    if (Input::isKeyDown(GLFW_KEY_V)) {
        Camera::activeCamera = &ecs.componentManager.get<CameraComponent>(mainCamera).camera;
    }
}

void Scene2::update(const float deltaTime) {

    TransformComponent& transform = ecs.componentManager.get<TransformComponent>(gun);
    const Camera& camera = ecs.componentManager.get<CameraComponent>(mainCamera).camera;

    // transform.local_position = glm::vec3(0.35f, -0.5f, -1.0f);
    // transform.local_rotation = glm::vec3(0.0f, 95.0f, 5.0f);

    transform.local_position = {
        camera.right_axis  * -0.35f -
        camera.up_axis     * 0.5f +
        camera.getTarget() * 1.0f
    };

    const float GUN_OFFSET_Y = 4.0f;
    const float GUN_OFFSET_Z = 2.0f;

    transform.local_rotation = {
        0.0f,
        camera.yaw   * -1.0f + GUN_OFFSET_Y,
        camera.pitch *  1.0f + GUN_OFFSET_Z
    };
    
    updateTransform(ecs);
}

void Scene2::render() const {

    Renderer::disableCulling();
    RenderSystem::instance().render(ecs);
}

void Scene2::renderLight() const {
    
    RenderSystem::instance().renderLights(ecs);

    static bool useEnv = true;

    useEnv = Input::isKeyDown(GLFW_KEY_G)? !useEnv : useEnv;

    if (useEnv) {

        glDepthFunc(GL_LEQUAL);

        const Shader& shader = *Shaders::get(ENVIRONMENT);
        shader.use();

        shader.setMat4("projection", Camera::get_projection());
        shader.setMat4("view", glm::mat4(glm::mat3(Camera::get_view())));
        
        shader.setInt("environmentMap", 0);
        env->bindTexture(0);

        env->draw();
        glDepthFunc(GL_LESS);
    }

    const CameraComponent& cameraComponent = ecs.componentManager.get<CameraComponent>(mainCamera);
    if (Camera::activeCamera != &cameraComponent.camera) cameraComponent.renderFrustum();

    // const auto& cam_pos = cameraComponent.camera.getPos();
    // auto gun_pos = ecs.componentManager.get<TransformComponent>(gun).position;

    // if (Input::isKeyDown(GLFW_KEY_LEFT_BRACKET)) gun_pos.y -= 0.01f;
    // if (Input::isKeyDown(GLFW_KEY_RIGHT_BRACKET)) gun_pos.y += 0.01f;
    // DebugMenu::log("y: " + std::to_string(gun_pos.y));

    // gun_pos.y = -0.18f;

    // glm::vec3 direction = glm::rotateY(glm::vec3(0.0f, 0.0f,-1.0f), glm::radians(5.0f));

    // Line::render(cam_pos, gun_pos);
    // Line::renderDirection(
    //     gun_pos,
    //     // glm::vec3(0.0f, 0.0f, 3.0f),
    //     direction,
    //     5.0f
    // );
}

void Scene2::renderPointShadow() const {

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Renderer::disableCulling();

    ShadowSystem::instance().render(ecs);
}

void Scene2::renderDirectShadow() const {

    ShadowSystem::instance().renderDirectional(ecs);
}

void Scene2::renderGbuffer() const {

    RenderSystem::instance().renderGbuffer(ecs);
}

void Scene2::renderDeferred(const Shader& currentShader) const {

    RenderSystem::instance().lightningPass(ecs, &currentShader);
}