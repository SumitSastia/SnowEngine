#include "scenes.h"

#include <glm/gtx/rotate_vector.hpp>

#include "core/camera.h"
#include "core/input.h"
#include "core/renderer.h"

#include "ecs/systems/update.h"
#include "core/img/cubemap.h"
#include "core/img/assetManager.h"

#include "math/random.h"
#include "utils/debug.h"

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
    campfire  = entityManager.createEntity();
    flames    = entityManager.createEntity();

    mainCamera = entityManager.createEntity();

    entityManager.emissiveEntities = {
        light1,
        light2
    };

    entityManager.transparentEntities = {
        flames
    };

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
        // transform.scale    = glm::vec3(0.1f);
        transform.computeModel();

        // transform.isVisible = false;
        
        MeshComponent mesh = EntityShapes::instance().cubeNorm;
        
        DebugMenu::log("EntityShapes (init): " + running::globalTimer::endInterval());
        
        MaterialComponent material;
        material.shader = gfx::shader::S3D_PBR_NORM;
        
        material.albedo = AssetManager::loadTexture("assets/textures/wood_box.png");
        // material.albedo = AssetManager::loadTexture_flatColor(glm::vec4(colors::WHITE, 1.0f));
        material.normal = AssetManager::loadTexture("assets/textures/wood_box_normal.png", 1);
        
        material.gbufferShader = gfx::shader::S3D_GBUFFER_NORM;
        
        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = transform.position;
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        BoundingAABBComponent AABB = createAABB(transform, mesh);
        
        componentManager.addComponent(wood_box, mesh);
        componentManager.addComponent<TransformComponent>(wood_box);
        // componentManager.addComponent(wood_box, transform);
        componentManager.addComponent(wood_box, material);
        componentManager.addComponent(wood_box, boundingSphere);
        componentManager.addComponent(wood_box, AABB);
    }

    DebugMenu::log("WoodBox: " + running::globalTimer::endInterval());

    // floor
    {
        TransformComponent transform;
        transform.position = glm::vec3(0.0f, -2.0f, 0.0f);
        transform.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
        transform.scale    = glm::vec3(10.0f);
        transform.computeModel();

        MeshComponent mesh = EntityShapes::instance().square;

        MaterialComponent material;
        // material.shader = ShaderManager::get(NORMPBR2D);
        // material.shader = &ShaderManager::get(gfx::shader::S2D_PBR_NORM);
        material.shader = gfx::shader::S2D_PBR_NORM;

        material.albedo   = AssetManager::loadTexture("assets/textures/brickwall.jpg");
        material.normal   = AssetManager::loadTexture("assets/textures/brickwall_normal.png", 1);
        material.specular = AssetManager::loadTexture("assets/textures/stone_floor.jpg");

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        // material.gbufferShader = ShaderManager::get(GBUFFERNORM_2D);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S2D_GBUFFER_NORM);
        material.gbufferShader = gfx::shader::S2D_GBUFFER_NORM;

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
        // material.shader   = ShaderManager::get(NORMPBR2D);
        // material.shader = &ShaderManager::get(gfx::shader::S2D_PBR_NORM);
        material.shader = gfx::shader::S2D_PBR_NORM;

        material.albedo   = componentManager.get<MaterialComponent>(floor).albedo;
        material.normal   = componentManager.get<MaterialComponent>(floor).normal;
        material.specular = componentManager.get<MaterialComponent>(floor).specular;

        // material.gbufferShader = ShaderManager::get(GBUFFERNORM_2D);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S2D_GBUFFER_NORM);
        material.gbufferShader = gfx::shader::S2D_GBUFFER_NORM;

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
        // material.shader = ShaderManager::get(PARALLAX2D);
        // material.gbufferShader = ShaderManager::get(GBUFFERNORM_2D);

        // material.shader = &ShaderManager::get(gfx::shader::S2D_PARALLAX);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S2D_GBUFFER_NORM);

        material.shader = gfx::shader::S2D_PARALLAX;
        material.gbufferShader = gfx::shader::S2D_GBUFFER_NORM;
        
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
    {
        TransformComponent transform;
        transform.position = glm::vec3(6.0f, 0.0f, 3.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(1.0f);
        transform.computeModel();

        MaterialComponent material;
        // material.shader = ShaderManager::get(NORMPBR3D);
        // material.gbufferShader = ShaderManager::get(GBUFFER3D);

        // material.shader = &ShaderManager::get(gfx::shader::S3D_PBR_NORM);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S3D_GBUFFER_NORM);

        material.shader = gfx::shader::S3D_PBR_NORM;
        material.gbufferShader = gfx::shader::S3D_GBUFFER_NORM;

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        ModelComponent model0 = AssetManager::loadModel("assets/models/sphere/sphereHD.obj", true);
        ModelComponent model1 = AssetManager::loadModel("assets/models/sphere/sphere.obj", true);
        ModelComponent model2 = AssetManager::loadModel("assets/models/sphere/sphereSD.obj", true);
        
        ModelLODComponent meshLOD { model0, model1, model2 };

        BoundingAABBComponent AABB = createAABB(transform, model0);

        componentManager.addComponent(sphere, meshLOD);
        componentManager.addComponent(sphere, transform);
        componentManager.addComponent(sphere, material);
        componentManager.addComponent(sphere, AABB);
    }

    // {
    //     TransformComponent transform;
    //     transform.position = glm::vec3(6.0f, 2.0f, 3.0f);
    //     transform.rotation = glm::vec3(0.0f);
    //     transform.scale    = glm::vec3(1.0f);
    //     transform.computeModel();

    //     MaterialComponent material;
    //     material.shader        = ShaderManager::get(NORMPBR3D);
    //     material.gbufferShader = ShaderManager::get(GBUFFER3D);

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
        // material.shader = ShaderManager::get(PHONG3D);
        // material.gbufferShader = ShaderManager::get(GBUFFER3D);

        // material.shader = &ShaderManager::get(gfx::shader::S3D_PHONG);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S3D_GBUFFER);

        material.shader = gfx::shader::S3D_PHONG;
        material.gbufferShader = gfx::shader::S3D_GBUFFER;

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
        // material.shader = ShaderManager::get(INSTANCEPBR3D);
        // material.shader = &ShaderManager::get(gfx::shader::S3D_PBR_INSTANCE);
        material.shader = gfx::shader::S3D_PBR_INSTANCE;

        material.albedo = AssetManager::loadTexture("assets/textures/grunge-box-small.jpg");

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        // material.gbufferShader = ShaderManager::get(INSTANCE_GBUFFER3D);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S3D_GBUFFER_INSTANCE);
        material.gbufferShader = gfx::shader::S3D_GBUFFER_INSTANCE;

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
        // material.shader = ShaderManager::get(LIGHT3D);
        // material.shader = &ShaderManager::getUtil(gfx::shader::LIGHT);
        material.shader = gfx::shader::S3D_LIGHT;

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
        // material.shader = ShaderManager::get(LIGHT3D);
        // material.shader = &ShaderManager::getUtil(gfx::shader::LIGHT);
        material.shader = gfx::shader::S3D_LIGHT;

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

    DebugMenu::log("Light2: " + running::globalTimer::endInterval());

    // Campfire
    {
        TransformComponent transform;
        transform.position = glm::vec3(-1.0f, -1.7f, 1.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.4f);

        // transform.local_position = glm::vec3(0.35f, -0.5f, -1.0f);
        // transform.local_rotation = glm::vec3(0.0f, 95.0f, 5.0f);
        // transform.local_scale    = glm::vec3(0.2f);

        transform.computeModel();

        MaterialComponent material;
        // material.shader = ShaderManager::get(PBR3D);
        // material.gbufferShader = ShaderManager::get(GBUFFER3D);

        // material.shader = &ShaderManager::get(gfx::shader::S3D_PBR);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S3D_GBUFFER);

        material.shader = gfx::shader::S3D_PBR;
        material.gbufferShader = gfx::shader::S3D_GBUFFER;

        ModelComponent model = AssetManager::loadModel("assets/models/campfire/campfire.obj");
        
        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingAABBComponent AABB = createAABB(transform, model);

        componentManager.addComponent(campfire, model);
        componentManager.addComponent(campfire, material);
        componentManager.addComponent(campfire, transform);
        componentManager.addComponent(campfire, AABB);
    }

    // flameSprite
    {
        TransformComponent transform;
        transform.position = glm::vec3(-1.0f, -0.95f, 1.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(1.0f);

        // transform.isVisible = false;

        transform.computeModel();

        MeshComponent mesh = EntityShapes::instance().square;

        MaterialComponent material;
        // material.shader = ShaderManager::get(ALBEDO2D);
        // material.gbufferShader = ShaderManager::get(GBUFFER2D);

        // material.shader = &ShaderManager::get(gfx::shader::S2D_ALBEDO);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S2D_GBUFFER);

        material.shader = gfx::shader::S2D_ALBEDO;
        material.gbufferShader = gfx::shader::S2D_GBUFFER;
        
        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingAABBComponent AABB = createAABB(transform, mesh);

        AnimatedSprite animSprite;
        animSprite.sprites = {

            AssetManager::loadTexture("assets/sprites/flame1.png"),
            AssetManager::loadTexture("assets/sprites/flame2.png"),
            AssetManager::loadTexture("assets/sprites/flame3.png"),
            AssetManager::loadTexture("assets/sprites/flame4.png"),
            AssetManager::loadTexture("assets/sprites/flame5.png"),
            AssetManager::loadTexture("assets/sprites/flame6.png"),
            AssetManager::loadTexture("assets/sprites/flame7.png"),
            AssetManager::loadTexture("assets/sprites/flame8.png")
        };

        animSprite.activeSprite  = animSprite.sprites[0];
        animSprite.total_sprites = animSprite.sprites.size();

        animSprite.transition_rate = 0.1f;

        componentManager.addComponent(flames, mesh);
        componentManager.addComponent(flames, material);
        componentManager.addComponent(flames, transform);
        componentManager.addComponent(flames, animSprite);
        componentManager.addComponent(flames, AABB);
    }

    DebugMenu::log("Flame Sprite: " + running::globalTimer::endInterval());

    // gun
    {
        TransformComponent transform;
        transform.position = glm::vec3(0.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.2f);

        transform.local_position = glm::vec3(0.35f, -0.5f, -1.0f);
        // transform.local_rotation = glm::vec3(0.0f, 95.0f, 5.0f);
        transform.local_scale    = glm::vec3(0.2f);

        transform.computeModel();

        MaterialComponent material;
        // material.shader = ShaderManager::get(PHONG3D);
        // material.gbufferShader = ShaderManager::get(GBUFFER3D);

        // material.shader = &ShaderManager::get(gfx::shader::S3D_PHONG);
        // material.gbufferShader = &ShaderManager::get(gfx::shader::S3D_GBUFFER);

        material.shader = gfx::shader::S3D_PHONG;
        material.gbufferShader = gfx::shader::S3D_GBUFFER;

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

    // Position: 0.189843,0.173202,-0.00755653 - gunscale = 0.1
    // Position: 0.317998,0.342213,0 - gunscale = 0.2

    // Relative to {0,0,0}
    gun_muzzle = { 0.317998f, 0.342213f, 0.0f };

    ChildComponent camChild;
    camChild.children = { gun };
    componentManager.addComponent(mainCamera, camChild);

    // ----- Particles ----- //

    // CampFire
    Particle fire = gfx::particles::Fire;
    const glm::vec3 fire_variation = glm::vec3(0.2f, 0.0f, 0.2f);
    
    fire.position = ecs.componentManager.get<TransformComponent>(campfire).position;
    fire.position.y += 0.2f;
    
    // fire_emitter.softness  = 0.5f;
    fire_emitter.isLooping = true;
    fire_emitter.renderInstance = true;

    ParticleInitProperties property = gfx::particles::FireProperties;
    property.center = fire.position;
    property.total_count = 10;

    // Smoke
    Particle smoke = gfx::particles::smoke;
    ParticleInitProperties smoke_property = gfx::particles::smokeProperties;
    smoke_property.center = fire.position + glm::vec3(0.0f, 1.0f, 0.0f);
    smoke_property.total_count = 100;
    
    smoke_emitter.softness  = 0.8f;
    smoke_emitter.isLooping = true;

    smoke_emitter.particleType = static_cast<bool>(gfx::particles::Type::TEXTURED);
    smoke_emitter.setTexture(AssetManager::loadTexture("assets/particles/smoke_01.png", true));

    fire_emitter.create(fire, property);
    smoke_emitter.create(smoke, smoke_property);

    smoke_emitter.effects.forces = {

        gfx::particles::wind,
        gfx::particles::buoyancy,
        gfx::particles::turbulence
    };

    // LAST INITIALIZATION WORK //

    DebugMenu::printAssetsInitTime();

    // Multi-threaded Work
    AssetManager::compileTextures();
    DebugMenu::log("Texture Compilation: " + running::globalTimer::endInterval());
}

void Scene2::input(GLFWwindow* window, const float& delta_time) {

    static Entity entity = 0;
    const  float  move_speed = scene_var::speed * delta_time;

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

    const Camera& camera = ecs.componentManager.get<CameraComponent>(mainCamera).camera;

    const auto& view = camera.getView();

    const glm::vec3 cameraRight(view[0][0], view[1][0], view[2][0]);
    const glm::vec3 cameraUp   (view[0][1], view[1][1], view[2][1]);
    const glm::vec3 cameraFront(view[0][2], view[1][2], view[2][2]);

    // Animated Sprites
    updateAnimSprites(ecs.componentManager.get<AnimatedSprite>(flames), deltaTime);

    TransformComponent& flameTransform = ecs.componentManager.get<TransformComponent>(flames);

    glm::mat4 matrix;

    matrix[0] = glm::vec4(cameraRight * flameTransform.scale.x, 0.0f);
    matrix[1] = glm::vec4(cameraUp    * flameTransform.scale.y, 0.0f);
    matrix[2] = glm::vec4(cameraFront            , 0.0f);
    matrix[3] = glm::vec4(flameTransform.position, 1.0f);

    flameTransform.model.setMatrix(matrix);
    flameTransform.computePosition();

    // FPS Gun
    TransformComponent& transform = ecs.componentManager.get<TransformComponent>(gun);

    transform.local_position = {
        cameraRight * 0.35f -
        cameraUp    * 0.5f +
        cameraFront * -1.0f
    };

    const float GUN_OFFSET_Y = 4.0f;
    const float GUN_OFFSET_Z = 2.0f;

    transform.local_rotation = {
        0.0f,
        camera.getYaw()   * -1.0f + GUN_OFFSET_Y,
        camera.getPitch() *  1.0f + GUN_OFFSET_Z
    };

    updateTransform(ecs);

    // Particles
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {

        TransformComponent muzzle_transform;
        muzzle_transform.position = transform.position;
        muzzle_transform.rotation = transform.rotation;

        muzzle_transform.computeModel();

        Particle flash = gfx::particles::flash;
        flash.position = glm::vec3(muzzle_transform.model * glm::vec4(gun_muzzle, 1.0f));
        
        ParticleInitProperties flash_properties = gfx::particles::flashProperties;
        flash_properties.center = flash.position;

        const glm::vec3 cam_direction = glm::normalize(camera.getTarget());

        flash_properties.velocity_min = 5.0f * cam_direction;
        flash_properties.velocity_max = 5.0f * cam_direction;

        flash_properties.acc_min = 2.0f * cam_direction;
        flash_properties.acc_max = 2.0f * cam_direction;

        flash_properties.acc_min = flash.acceleration;
        flash_properties.acc_max = flash.acceleration;

        flash_properties.total_count = 10;

        bullet_emitter.create(flash, flash_properties);
    }

    // Rain
    Particle rain = gfx::particles::Rain;
    ParticleInitProperties rain_property = gfx::particles::RainProperties;

    rain_property.center = camera.getPos();
    rain_property.total_count = 2;

    rain_emitter.create(rain, rain_property);

    fire_emitter.update(deltaTime);
    rain_emitter.update(deltaTime);
    bullet_emitter.update(deltaTime);
    smoke_emitter.update(deltaTime);

    // const glm::vec3& position = ecs.componentManager.get<TransformComponent>(wood_box).position;
    // std::cout << "Position: " << position.x << "," << position.y << "," << position.z << '\n';
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

        const Shader& shader = ShaderManager::getUtil(gfx::shader::ENVIRONMENT);
        shader.use();

        shader.setMat4("projection", Camera::activeCamera->getProjection());
        shader.setMat4("view", glm::mat4(glm::mat3(Camera::activeCamera->getView())));
        
        shader.setInt("environmentMap", 0);
        env->bindTexture(0);

        env->draw();
        glDepthFunc(GL_LESS);
    }

    const CameraComponent& cameraComponent = ecs.componentManager.get<CameraComponent>(mainCamera);
    if (Camera::activeCamera != &cameraComponent.camera) cameraComponent.renderFrustum();

    RenderSystem::instance().renderTransparent(ecs);
}

void Scene2::renderParticles(const TextureHandle depthTexture) const {

    fire_emitter.render(depthTexture);
    rain_emitter.render(depthTexture);
    bullet_emitter.render(depthTexture);
    smoke_emitter.render(depthTexture);
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

void Scene2::renderDeferred() const {

    RenderSystem::instance().lightningPass(ecs);
}