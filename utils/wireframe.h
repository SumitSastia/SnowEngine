#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gfx::internal {

    class Wireframe {

        uint VBO, VAO, EBO;
        uint indicesCount;

    public:

        void init(
            const std::vector<float>& vertices,
            const std::vector<uint>&  indices
        );

        void draw() const;
    };
}


struct Wireframes {

    gfx::internal::Wireframe sphere;
    gfx::internal::Wireframe cube;

    Wireframes();

    static const Wireframes& instance() {
        static Wireframes instance {};
        return instance;
    } 
};