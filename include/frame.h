#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace gfx::internal {

	class Screen {
		
        static unsigned int vbo, vao;

        Screen() {
			const float vertices[] = {

				// Position  // Cords
				-1.0f, 1.0f, 0.0f, 1.0f,
				 1.0f, 1.0f, 1.0f, 1.0f,
				-1.0f,-1.0f, 0.0f, 0.0f,

				 1.0f, 1.0f, 1.0f, 1.0f,
				 1.0f,-1.0f, 1.0f, 0.0f,
				-1.0f,-1.0f, 0.0f, 0.0f
			};

			glGenBuffers(1, &vbo);
			glGenVertexArrays(1, &vao);

			glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);
		}

	public:

		static const unsigned int getVAO() {

			static Screen instance {};
			return instance.vao;
		}
	};
}

class FrameBuffer {

    unsigned int fbo, rbo;
    unsigned int texture_id;
    unsigned int shader;

public:

    virtual void init() = 0;
    virtual void render() const = 0;
    virtual void destroy() = 0;
};

namespace frameBuffers {

    const unsigned int get_defaultVAO();
}