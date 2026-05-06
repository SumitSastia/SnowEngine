#pragma once

#include <shader.h>
#include <shapes.h>
#include <entity.h>

#include <vector>

class Scene {

protected:

    uint8_t total_shaders;
    uint16_t total_entities;

    std::vector <Shader> loaded_shaders;
    std::vector <Entity> loaded_entities;

    Scene() : total_shaders(0) {}

public:

    virtual void init() = 0;
    virtual void render() const = 0;
    virtual void destroy() = 0;
};

class Scene1 : public Scene {

    // Entities

    Shape square;
    Shape myCube;
    ShapeInstanced cubes;

public:

    Scene1() {
        this->init();
    }

    void init() override;
    void render() const override;
    void destroy() override;
};