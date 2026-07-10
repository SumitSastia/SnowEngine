#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <core/config.h>

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

namespace frameBuffers {

	const uint16_t shadowSize = 1024;
	const unsigned int get_defaultVAO();
	void renderScreen();
}

class FrameBuffer {

protected:

    unsigned int fbo;

	UintRes width;
	UintRes height;

public:

	const UintRes& getWidth()  const { return width; }
	const UintRes& getHeight() const { return height; }

	void bindFBO() const { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }

	const unsigned int getFBO() const { return fbo; }

    virtual void init() = 0;
    virtual void render() const {};
    virtual void destroy();
};

class DepthFrame : public FrameBuffer {

	unsigned int depth_texture;

public:

	void create(const UintRes frameWidth, const UintRes frameHeight);

	void init() override {}
	void render() const override;
};

class DebugFrame : public FrameBuffer {

	unsigned int rbo;
    unsigned int texture_id;

public:

	DebugFrame(const uint16_t& frameWidth, const uint16_t& frameHeight);

	void init() override {}
	void render() const override;
	void render(const unsigned int& textureID) const;
};

class DirectShadowFrame : public FrameBuffer {

    unsigned int texture_id;

public:

	unsigned int getID() const { return texture_id; }

	DirectShadowFrame(const uint16_t& shadow_size);
	DirectShadowFrame() : DirectShadowFrame(frameBuffers::shadowSize) {}

	void bindTexture(const unsigned int textureUnit) const;

	void init() override {}
	void render() const override {}
};

class PointShadowFrame : public FrameBuffer {

    unsigned int texture_id;

public:

	PointShadowFrame(const uint16_t& shadow_size);
	PointShadowFrame() : PointShadowFrame(frameBuffers::shadowSize) {}

	void bindTexture(const unsigned int textureUnit) const;

	void init() override {}
	void render() const override {}
};

class HDRFrame : public FrameBuffer {

public:

	HDRFrame(const UintRes& frameWidth, const UintRes& frameHeight);

	void init() override {}
	void render() const override;

	const unsigned int& getDepthTexture() const { return depth_texture; }

private:

	unsigned int rbo;
    unsigned int color_texture;
	unsigned int depth_texture;
};

class BlurFrame {

public:
	
	unsigned int pingpongFBO[2];
	unsigned int pingpongTex[2];
	
	BlurFrame(const uint16_t& frameWidth, const uint16_t& frameHeight);
};

class BloomFrame : public FrameBuffer {

	unsigned int rbo;
	unsigned int colorBuffers[2];
	
	BlurFrame* _blur;

public:

	BloomFrame(const uint16_t& frameWidth, const uint16_t& frameHeight);

	void init() override {}
	void render() const;
};

class Gbuffer : public FrameBuffer {

	unsigned int rbo;
	unsigned int gPosition, gNormal, gTexture;

public:

	Gbuffer(const uint16_t& frameWidth, const uint16_t& frameHeight);

	void init() override {}
	void render() const override;

	// for SSAO
	void bind_gPosition(const unsigned int textureUnit) const;
	void bind_gNormal(const unsigned int textureUnit) const;
};