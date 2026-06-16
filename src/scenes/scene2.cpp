#include <scenes.h>
#include <camera.h>
#include <renderer.h>
#include <debug.h>
#include <input.h>

void Scene2::init() {

    EntityManager&    entityManager    = ecs.entityManager;
    ComponentManager& componentManager = ecs.componentManager;

    running::globalTimer::startInterval();

    wood_box  = ecs.entityManager.createEntity();
    floor     = ecs.entityManager.createEntity();
    wall      = ecs.entityManager.createEntity();
    light1    = ecs.entityManager.createEntity();
    light2    = ecs.entityManager.createEntity();
    cubes     = ecs.entityManager.createEntity();
    sphere    = ecs.entityManager.createEntity();
    brickWall = ecs.entityManager.createEntity();
    headcam   = ecs.entityManager.createEntity();
    sun       = ecs.entityManager.createEntity();

    entityManager.visibleEntities.push_back(wood_box);
    entityManager.visibleEntities.push_back(floor);
    entityManager.visibleEntities.push_back(wall);
    entityManager.visibleEntities.push_back(cubes);
    entityManager.visibleEntities.push_back(sphere);
    entityManager.visibleEntities.push_back(brickWall);
    entityManager.visibleEntities.push_back(headcam);

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
    std::vector <std::string> envFaces = {
        
        "assets/env/right.png",
        "assets/env/left.png",
        "assets/env/top.png",
        "assets/env/bottom.png",
        "assets/env/front.png",
        "assets/env/back.png"
    };

    // env = new Environment(envFaces);
    env = new Environment("assets/env/hdri-sky.hdr", 1024);
    entityManager.env = env;

    DebugMenu::log("HDR Environment: " + running::globalTimer::endInterval());

    // wood_box
    {
        TransformComponent transform;
        transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(1.0f);
        transform.computeModel();
        
        MeshComponent mesh;
        mesh = EntityShapes::instance().cubeNorm;
        
        DebugMenu::log("EntityShapes (init): " + running::globalTimer::endInterval());
        
        MaterialComponent material;
        material.shader = Shaders::get(NORMPBR3D);
        
        material.albedo = new Texture2D("assets/textures/wood_box.png", 1);
        material.normal = new Texture2D("assets/textures/wood_box_normal.png");
        
        material.gbufferShader = Shaders::get(GBUFFERNORM_3D);
        
        DebugMenu::log("DefaultShapes (init): " + running::globalTimer::endInterval());
        
        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        // DebugMenu::log("Box center: " + vec3Str(boundingSphere.center));
        // DebugMenu::log("Box radius: " + std::to_string(boundingSphere.radius));
        
        componentManager.addComponent(wood_box, mesh);
        componentManager.addComponent(wood_box, transform);
        componentManager.addComponent(wood_box, material);
        componentManager.addComponent(wood_box, boundingSphere);
    }

    DebugMenu::log("WoodBox: " + running::globalTimer::endInterval());

    // for (uint32_t i = 0; i < 1000; i++) {

    //     crowd[i] = entityManager.createEntity();

    //     TransformComponent transform;
    //     transform.position = randomPosition(-20.0f, 20.0f);
    //     transform.rotation = glm::vec3(0.0f);
    //     transform.scale    = glm::vec3(1.0f);
    //     transform.computeModel();
        
    //     MeshComponent mesh;
    //     // mesh.shape = EntityShapes::instance().cube;
    //     mesh.shape = EntityShapes::instance().cubeNormalMapped;
        
    //     MaterialComponent material;
    //     material.shader = Shaders::get(NORMPBR3D);
        
    //     material.albedo = componentManager.get<MaterialComponent>(wood_box).albedo;
    //     material.normal = componentManager.get<MaterialComponent>(wood_box).normal;
        
    //     material.gbufferShader = Shaders::get(GBUFFERNORM_3D);
        
    //     material.metallic  = material.albedo;
    //     material.roughness = material.albedo;

    //     BoundingSphereComponent boundingSphere;
    //     boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    //     boundingSphere.radius = mesh.shape.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});
        
    //     componentManager.addComponent(crowd[i], mesh);
    //     componentManager.addComponent(crowd[i], transform);
    //     componentManager.addComponent(crowd[i], material);
    //     componentManager.addComponent(crowd[i], boundingSphere);
    // }

    DebugMenu::log("crowd: " + running::globalTimer::endInterval());

    // floor
    {
        TransformComponent transform;
        transform.position = glm::vec3(0.0f, -2.0f, 0.0f);
        transform.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
        transform.scale    = glm::vec3(10.0f);
        transform.computeModel();

        MeshComponent mesh;
        mesh = EntityShapes::instance().square;

        MaterialComponent material;
        material.shader = Shaders::get(NORMPBR2D);

        material.albedo = new Texture2D("assets/textures/brickwall.jpg", 1);
        material.normal = new Texture2D("assets/textures/brickwall_normal.png");

        material.gbufferShader = Shaders::get(GBUFFERNORM_2D);

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});
        
        componentManager.addComponent(floor, mesh);
        componentManager.addComponent(floor, transform);
        componentManager.addComponent(floor, material);
        componentManager.addComponent(floor, boundingSphere);
    }

    DebugMenu::log("Floor: " + running::globalTimer::endInterval());

    // wall
    {
        TransformComponent transform;
        transform.position = glm::vec3(-2.5f, 0.5f, -5.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(5.0f);
        transform.computeModel();

        MeshComponent mesh;
        mesh = EntityShapes::instance().square;

        MaterialComponent material;
        material.shader = Shaders::get(NORMPBR2D);
        material.albedo = componentManager.get<MaterialComponent>(floor).albedo;
        material.normal = componentManager.get<MaterialComponent>(floor).normal;

        material.gbufferShader = Shaders::get(GBUFFERNORM_2D);

        material.metallic  = material.albedo;
        material.roughness = material.albedo;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});
        
        componentManager.addComponent(wall, mesh);
        componentManager.addComponent(wall, transform);
        componentManager.addComponent(wall, material);
        componentManager.addComponent(wall, boundingSphere);
    }

    DebugMenu::log("Wall: " + running::globalTimer::endInterval());

    // Parallax wall
    {
        TransformComponent transform;
        transform.position = glm::vec3(2.5f, 0.5f, -5.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(2.0f);
        transform.computeModel();

        MeshComponent mesh;
        mesh = EntityShapes::instance().square;

        MaterialComponent material;
        // material.shader = Shaders::get(NORM_PHONG2D);
        material.shader = Shaders::get(PARALLAX2D);
        // material.gbufferShader = Shaders::get(GBUFFERNORM_2D);
        
        material.albedo = new Texture2D("assets/textures/parallax_maps/bricks2.jpg", 1);
        material.normal = new Texture2D("assets/textures/parallax_maps/bricks2_normal.jpg");
        material.height = new Texture2D("assets/textures/parallax_maps/bricks2_disp.jpg");

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});
        
        componentManager.addComponent(brickWall, mesh);
        componentManager.addComponent(brickWall, transform);
        componentManager.addComponent(brickWall, material);
        componentManager.addComponent(brickWall, boundingSphere);
    }

    DebugMenu::log("Parallax Wall (albedo, normal, height): " + running::globalTimer::endInterval());

    // sphere
    float sphereRadius = 0.0f;
    {
        TransformComponent transform;
        transform.position = glm::vec3(6.0f, 0.0f, 3.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(1.0f);
        transform.computeModel();

        Model3D* sphere0 = new Model3D("../assets/models/sphere/sphereHD.obj");
        Model3D* sphere1 = new Model3D("../assets/models/sphere/sphere.obj");
        Model3D* sphere2 = new Model3D("../assets/models/sphere/sphereSD.obj");

        MaterialComponent material;
        material.shader = Shaders::get(PBR3D);
        material.gbufferShader = Shaders::get(GBUFFER3D);
        componentManager.addShader(material.shader);

        ModelComponent model0 (sphere0, material);
        ModelComponent model1 (sphere1, material);
        ModelComponent model2 (sphere2, material);
        
        ModelLODComponent meshLOD { {sphere0, material}, model1, model2 };
        
        sphereRadius = sphere0->clean();

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = sphereRadius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        componentManager.addComponent(sphere, meshLOD);
        componentManager.addComponent(sphere, transform);
        componentManager.addComponent(sphere, boundingSphere);
    }

    for (uint32_t i = 0; i < 100; i++) {

        crowd[i] = entityManager.createEntity();

        TransformComponent transform;
        transform.position = randomPosition(-20.0f, 20.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(1.0f);
        transform.computeModel();

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = sphereRadius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        componentManager.addComponent(crowd[i], componentManager.get<ModelLODComponent>(sphere));
        componentManager.addComponent(crowd[i], transform);
        componentManager.addComponent(crowd[i], boundingSphere);
    }

    // headcam
    {
        TransformComponent transform;
        transform.position = glm::vec3(1.0f, 0.0f, 2.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.5f);
        transform.computeModel();

        Model3D* camModel = new Model3D("../assets/models/test_cube/colorCamera.obj");

        MaterialComponent material;
        material.shader = Shaders::get(PHONG3D);
        material.gbufferShader = Shaders::get(GBUFFER3D);
        componentManager.addShader(material.shader);

        ModelComponent modelComponent;
        modelComponent.init(camModel, material);

        float localRadius = camModel->clean();

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = localRadius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        componentManager.addComponent(headcam, modelComponent);
        componentManager.addComponent(headcam, transform);
        componentManager.addComponent(headcam, boundingSphere);
    }

    DebugMenu::log("Sphere (Model3D to ECS): " + running::globalTimer::endInterval());

    // cubes
    {
        MaterialComponent material;
        material.shader = Shaders::get(INSTANCEPBR3D);
        material.albedo = new Texture2D("assets/textures/grunge-box-small.jpg", 1);

        material.gbufferShader = Shaders::get(INSTANCE_GBUFFER3D);
        
        componentManager.addComponent(cubes, EntityShapes::instance().cubes);
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

        MeshComponent mesh;
        mesh = EntityShapes::instance().cube;

        MaterialComponent material;
        material.shader = Shaders::get(LIGHT3D);

        PointLightComponent pointlight;
        pointlight.color     = colors::YELLOW;
        pointlight.constant  = DefaultLights::instance().cubelight.constant;
        pointlight.linear    = DefaultLights::instance().cubelight.linear;
        pointlight.quadratic = DefaultLights::instance().cubelight.quadratic;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        componentManager.addComponent(light1, mesh);
        componentManager.addComponent(light1, transform);
        componentManager.addComponent(light1, material);
        componentManager.addComponent(light1, pointlight);
        componentManager.addComponent(light1, boundingSphere);
    }

    DebugMenu::log("Light1: " + running::globalTimer::endInterval());

    // light2
    {
        TransformComponent transform;
        transform.position = glm::vec3(-3.0f, 2.5f, 3.0f);
        transform.rotation = glm::vec3(0.0f);
        transform.scale    = glm::vec3(0.5f);
        transform.computeModel();

        MeshComponent mesh;
        mesh = EntityShapes::instance().cube;

        MaterialComponent material;
        material.shader = Shaders::get(LIGHT3D);

        PointLightComponent pointlight;
        pointlight.color     = 2.0f * colors::PINK;
        pointlight.constant  = DefaultLights::instance().cubelight.constant;
        pointlight.linear    = DefaultLights::instance().cubelight.linear;
        pointlight.quadratic = DefaultLights::instance().cubelight.quadratic;

        BoundingSphereComponent boundingSphere;
        boundingSphere.center = glm::vec3(transform.model.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        boundingSphere.radius = mesh.radius * std::max({transform.scale.x, transform.scale.y, transform.scale.z});

        componentManager.addComponent(light2, mesh);
        componentManager.addComponent(light2, transform);
        componentManager.addComponent(light2, material);
        componentManager.addComponent(light2, pointlight);
        componentManager.addComponent(light2, boundingSphere);
    }

    DebugMenu::log("Light2: " + running::globalTimer::endInterval());
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

        shader.setMat4("projection", Camera::instance().getPerspective());
        shader.setMat4("view", glm::mat4(glm::mat3(Camera::instance().getView())));
        
        shader.setInt("environmentMap", 0);
        env->bindTexture(0);

        env->draw();
        glDepthFunc(GL_LESS);
    }
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