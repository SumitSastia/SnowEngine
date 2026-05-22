#include <frame.h>
#include <shader.h>
#include <renderer.h>
#include <input.h>

#include <iostream>

unsigned int gfx::internal::Screen::vao = 0;
unsigned int gfx::internal::Screen::vbo = 0;

namespace frameBuffers {

    const unsigned int get_defaultVAO() {
        return gfx::internal::Screen::getVAO();
    }
}

void FrameBuffer::destroy() {

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texture_id);
}

DebugFrame::DebugFrame(const uint16_t& frameWidth, const uint16_t& frameHeight) {

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Texture Attachment
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // Render Object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameWidth, frameHeight);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR :: UNABLE TO COMPLETE DEBUG-FRAME-BUFFER!" << std::endl;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->init();
}

void DebugFrame::init() {

    shader = new Shader(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/default_fb.frag"
    );
}

void DebugFrame::render() const {

    Renderer::disableDepth();

    shader->use();
    shader->setInt("screen", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(frameBuffers::get_defaultVAO());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    Renderer::enableDepth();
}

void DebugFrame::render(const unsigned int& textureID) const {

    Renderer::disableDepth();

    shader->use();
    shader->setInt("screen", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(frameBuffers::get_defaultVAO());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    Renderer::enableDepth();
}

DirectShadowFrame::DirectShadowFrame(const uint16_t& shadow_size) {

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_id, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: Directional Shadow framebuffer incomplete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

PointShadowFrame::PointShadowFrame(const uint16_t& shadow_size) {

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (unsigned int i = 0; i < 6; i++) {

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_id, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: Point Shadow framebuffer incomplete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

HDRFrame::HDRFrame(const uint16_t& frameWidth, const uint16_t& frameHeight) {

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Texture Attachment
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // Render Object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameWidth, frameHeight);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR :: UNABLE TO COMPLETE DEBUG-FRAME-BUFFER!" << std::endl;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->init();
}

void HDRFrame::init() {

    shader = new Shader(
        "../shaders/frameBuffs/default_fb.vert",
        "../shaders/frameBuffs/hdr_frame.frag"
    );
}

void HDRFrame::render() const {

    // Renderer::disableDepth();

    shader->use();
    shader->setInt("screen", 0);
    
    static bool toggle = true;
    
    if (Input::isKeyDown(GLFW_KEY_M)) {
        
        toggle = !toggle;
        if (toggle) std::cout << "ENABLED  :: Tone Mapping" << std::endl;
        else        std::cout << "DISABLED :: Tone Mapping" << std::endl;
    }

    shader->setBool("toggle", toggle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(frameBuffers::get_defaultVAO());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Renderer::enableDepth();
}