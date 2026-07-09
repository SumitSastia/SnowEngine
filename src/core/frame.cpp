#include <frame.h>
#include <shader.h>
#include <renderer.h>
#include <input.h>
#include <ssao.h>
#include <debug.h>

#include <iostream>

unsigned int gfx::internal::Screen::vao = 0;
unsigned int gfx::internal::Screen::vbo = 0;

namespace frameBuffers {

    const unsigned int get_defaultVAO() {
        return gfx::internal::Screen::getVAO();
    }

    void renderScreen() {
        
        glBindVertexArray(gfx::internal::Screen::getVAO());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

void FrameBuffer::destroy() {

    glDeleteFramebuffers(1, &fbo);
}

void DepthFrame::create(const UintRes frameWidth, const UintRes frameHeight) {

    width  = frameWidth;
    height = frameHeight;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: DepthFrame-Buffer incomplete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

void DebugFrame::render() const {

    Renderer::disableDepth();

    const Shader& shader = ShaderManager::getFrame(gfx::shader::FRAME_DEFAULT);

    shader.use();
    shader.setInt("screen", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(frameBuffers::get_defaultVAO());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    Renderer::enableDepth();
}

void DebugFrame::render(const unsigned int& textureID) const {

    Renderer::disableDepth();

    const Shader& shader = ShaderManager::getFrame(gfx::shader::FRAME_DEFAULT);

    shader.use();
    shader.setInt("screen", 0);

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

void DirectShadowFrame::bindTexture(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

PointShadowFrame::PointShadowFrame(const uint16_t& shadow_size) {

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    
    
    for (unsigned int i = 0; i < 6; i++) {
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT16,
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

void PointShadowFrame::bindTexture(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
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

    // shader = new Shader(
    //     "../shaders/frameBuffs/default_fb.vert",
    //     "../shaders/frameBuffs/hdr_frame.frag"
    // );

    // shader = new Shader();

    // shader->loadFromFile(
    //     "../shaders/frameBuffs/default_fb.vert",
    //     "../shaders/frameBuffs/hdr_frame.frag"
    // );
}

void HDRFrame::render() const {

    const Shader& shader = ShaderManager::getFrame(gfx::shader::FRAME_HDR);

    shader.use();
    shader.setInt("screen", 0);
    
    static bool  toggle   = false;
    static float gamma = 2.3f;
    
    if (Input::isKeyDown(GLFW_KEY_M)) {
        
        toggle = !toggle;
        if (toggle) std::cout << "ENABLED  :: Tone Mapping" << std::endl;
        else        std::cout << "DISABLED :: Tone Mapping" << std::endl;
    }

    if (Input::isKeyDown(GLFW_KEY_KP_SUBTRACT)) {
        
        if (gamma > 1.0f) {
            gamma -= 0.1f;
            std::cout << "gamma: " << gamma << std::endl;
        }
    }

    if (Input::isKeyDown(GLFW_KEY_KP_ADD)) {
        
        if (gamma < 3.0f) {
            gamma += 0.1f;
            std::cout << "gamma: " << gamma << std::endl;
        }
    }
    
    shader.setBool("toggle", toggle);
    shader.setFloat("gamma", gamma);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    frameBuffers::renderScreen();
}

BloomFrame::BloomFrame(const uint16_t& frameWidth, const uint16_t& frameHeight) {

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Texture Attachment
    glGenTextures(2, colorBuffers);

    for (uint8_t i = 0; i < 2; i++) {

        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    // Render Object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameWidth, frameHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR :: UNABLE TO COMPLETE DEBUG-FRAME-BUFFER!" << std::endl;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _blur = new BlurFrame(frameWidth, frameHeight);

    // shader = new Shader(
    //     "../shaders/frameBuffs/default_fb.vert",
    //     "../shaders/frameBuffs/bloom.frag"
    // );
}

void BloomFrame::render() const {

    bool horizontal = true;
    bool first_itr  = true;

    int amount = 5;

    // _blur->shader->use();

    const Shader& shaderBlur = ShaderManager::getFrame(gfx::shader::FRAME_BLUR);
    shaderBlur.use();

    for (unsigned int i = 0; i < amount; i++) {

        glBindFramebuffer(GL_FRAMEBUFFER, _blur->pingpongFBO[horizontal]);
        shaderBlur.setBool("horizontal", horizontal);

        glBindTexture(GL_TEXTURE_2D, first_itr ? colorBuffers[1] : _blur->pingpongTex[!horizontal]);

        glBindVertexArray(frameBuffers::get_defaultVAO());
        glDrawArrays(GL_TRIANGLES, 0, 6);

        horizontal = !horizontal;
        first_itr  = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    const Shader& shader = ShaderManager::getFrame(gfx::shader::FRAME_BLOOM);

    shader.use();
    shader.setInt("screen", 0);
    shader.setInt("bloom", 1);

    glBindVertexArray(frameBuffers::get_defaultVAO());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _blur->pingpongTex[0]);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

BlurFrame::BlurFrame(const uint16_t& frameWidth, const uint16_t& frameHeight) {

    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongTex);

    for (unsigned int i = 0; i < 2; i++) {

        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongTex[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTex[i], 0);
    }

    // shader = new Shader(
    //     "../shaders/frameBuffs/default_fb.vert",
    //     "../shaders/frameBuffs/blur.frag"
    // );
}

Gbuffer::Gbuffer(const uint16_t& frameWidth, const uint16_t& frameHeight) {

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &gPosition);
    glGenTextures(1, &gNormal);
    glGenTextures(1, &gTexture);

    // Position buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // Normal buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frameWidth, frameHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // Texture buffer
    glGenTextures(1, &gTexture);
    glBindTexture(GL_TEXTURE_2D, gTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameWidth, frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gTexture, 0);

    unsigned int attachments[3] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(3, attachments);

    // Render Object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameWidth, frameHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR :: UNABLE TO COMPLETE DEBUG-FRAME-BUFFER!" << std::endl;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader = new Shader();

    // shader->loadFromFile(
    //     "../shaders/frameBuffs/default_fb.vert",
    //     "../shaders/frameBuffs/deferred.frag", true
    // );
}

void Gbuffer::render() const {

    const int textureUnit = 0;

    const Shader& shader = ShaderManager::getFrame(gfx::shader::DEFERRED_LIGHTNING);

    shader.use();
    shader.setInt("gPosition",  textureUnit);
    shader.setInt("gNormal",    textureUnit + 1);
    shader.setInt("gTexture",   textureUnit + 2);
    shader.setInt("gOcclusion", textureUnit + 3);

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    
    glActiveTexture(GL_TEXTURE0 + textureUnit + 1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    
    glActiveTexture(GL_TEXTURE0 + textureUnit + 2);
    glBindTexture(GL_TEXTURE_2D, gTexture);
    
    if (SSAO::enable) {
        SSAO::bindOcclusion(textureUnit + 3);
    }

    shader.setBool("toggleAO", SSAO::enable);
    
    frameBuffers::renderScreen();
}

void Gbuffer::bind_gPosition(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, gPosition);
}

void Gbuffer::bind_gNormal(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, gNormal);
}