#include <shapes.h>
#include <camera.h>

#include <iostream>

//-------------------------------------------------------------------------------------//

coloredCube::coloredCube(const glm::vec3& color) :
    color(color)
{

    const float vertices[] = {

        // Position
        -0.5, 0.5, 0.5,
         0.5, 0.5, 0.5,
        -0.5,-0.5, 0.5,
         0.5,-0.5, 0.5,

        -0.5, 0.5,-0.5,
         0.5, 0.5,-0.5,
        -0.5,-0.5,-0.5,
         0.5,-0.5,-0.5,
    };

    const unsigned int indices[] = {

        0,1,2,
        1,3,2,

        5,4,7,
        4,6,7,

        4,0,6,
        0,2,6,

        1,5,3,
        5,7,3,

        4,5,0,
        5,1,0,

        7,6,3,
        6,2,3
    };

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    colorShader = createShader(
        "C:/Users/sumit/Documents/GitHub/OpenGLRenderer/shaders/colorCube/cube.vert",
        "C:/Users/sumit/Documents/GitHub/OpenGLRenderer/shaders/colorCube/cube.frag"
    );
}

coloredCube::~coloredCube() {

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void coloredCube::update(const float& delta_time) {

}

void coloredCube::render() const {

    // std::cout << VAO << std::endl;

    glUseProgram(colorShader);

    setMat4(colorShader, "projection", camera::instance().getPerspective());
    setMat4(colorShader, "view", camera::instance().getView());
    setMat4(colorShader, "model", glm::mat4(1.0f));

    setVec3(colorShader, "color", color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------//

shape2D::~shape2D() {

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void shape2D::bindVertices(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(float);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // TextureCords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void shape2D::loadTexture(const char* texturePath) {
    shape2DTexture.load(texturePath);
}

void shape2D::draw(const unsigned int& shader, const glm::mat4& model) const {

    glUseProgram(shader);
    glDisable(GL_CULL_FACE);

    // Vert Uniforms
    setMat4(shader, "finalMatrix", camera::instance().getPerspective() * camera::instance().getView() * model);
    setMat4(shader, "model", model);

    // Frag Uniforms
    glm::vec3 normal = glm::transpose(glm::inverse(glm::mat3(model))) * glm::vec3(0.0f, 0.0f, 1.0f);
    normal = glm::normalize(normal);

    setVec3(shader, "normal", normal);
    setVec3(shader, "viewPos", camera::instance().getPos());

    setSpotLight(shader, "s1", lights::lights::instance().flashlight);

    setInt(shader, "texture1", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shape2DTexture.getID());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_CULL_FACE);
}

void shape2D::draw_gbuffer(const unsigned int& shader, const glm::mat4& model) const {

    glUseProgram(shader);
    glDisable(GL_CULL_FACE);

    setMat4(shader, "projection", camera::instance().getPerspective());
    setMat4(shader, "view", camera::instance().getView());
    setMat4(shader, "model", model);

    setVec3(shader, "normal", glm::normalize(normal));

    setInt(shader, "texture1", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shape2DTexture.getID());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_CULL_FACE);
}

void shape2D::drawShadow() const {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------//

specShape::~specShape() {

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void shape::bindVertices(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(float);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void shape::loadTexture(const char* diffusePath) {
    shapeDiffuseTexture.load(diffusePath);
}

void shape::draw(const unsigned int& shader, const glm::mat4& model) const {

    glUseProgram(shader);

    setMat4(shader, "finalMatrix", camera::instance().getPerspective() * camera::instance().getView() * model);
    setMat4(shader, "model", model);
    setMat3(shader, "normalModel", glm::transpose(glm::inverse(glm::mat3(model))));
    setVec3(shader, "viewPos", camera::instance().getPos());

    setMaterial(shader, "m1");
    setSpotLight(shader, "s1", lights::lights::instance().flashlight);

    setInt(shader, "texture1", 0);
    setInt(shader, "texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void shape::draw_gbuffer(const unsigned int& shader, const glm::mat4& model) const {

    glUseProgram(shader);

    setMat4(shader, "projection", camera::instance().getPerspective());
    setMat4(shader, "view", camera::instance().getView());
    setMat4(shader, "model", model);
    setMat3(shader, "normalModel", glm::transpose(glm::inverse(glm::mat3(model))));

    setInt(shader, "texture1", 0);
    setInt(shader, "texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void specShape::loadTexture(const char* diffusePath, const char* specularPath) {

    shapeDiffuseTexture.load(diffusePath);
    shapeSpecularTexture.load(specularPath);
}

void specShape::draw(const unsigned int& shader, const glm::mat4& model) const {

    glUseProgram(shader);

    setMat4(shader, "finalMatrix", camera::instance().getPerspective() * camera::instance().getView() * model);
    setMat4(shader, "model", model);
    setMat3(shader, "normalModel", glm::transpose(glm::inverse(glm::mat3(model))));
    setVec3(shader, "viewPos", camera::instance().getPos());

    setMaterial(shader, "m1");
    setSpotLight(shader, "s1", lights::lights::instance().flashlight);

    setInt(shader, "texture1", 0);
    setInt(shader, "texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shapeSpecularTexture.getID());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void specShape::draw_gbuffer(const unsigned int& shader, const glm::mat4& model) const {

    glUseProgram(shader);

    setMat4(shader, "projection", camera::instance().getPerspective());
    setMat4(shader, "view", camera::instance().getView());
    setMat4(shader, "model", model);
    setMat3(shader, "normalModel", glm::transpose(glm::inverse(glm::mat3(model))));

    setInt(shader, "texture1", 0);
    setInt(shader, "texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shapeSpecularTexture.getID());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void shape::drawShadow() const {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------//

void shapeInstanced::bindVertices(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {

    indicesCount = size_i / sizeof(float);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void shapeInstanced::draw(const unsigned int& shader, const unsigned int& instanceCounts) const {

    glUseProgram(shader);

    setMat4(shader, "projection", camera::instance().getPerspective());
    setMat4(shader, "view", camera::instance().getView());

    setVec3(shader, "viewPos", camera::instance().getPos());
    setMaterial(shader, "m1");
    setSpotLight(shader, "s1", lights::lights::instance().flashlight);

    setInt(shader, "texture1", 0);
    setInt(shader, "texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shapeSpecularTexture.getID());

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, instanceCounts);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

void shapeInstanced::draw_gbuffer(const unsigned int& shader, const unsigned int& instanceCounts) const {

    glUseProgram(shader);

    setMat4(shader, "projection", camera::instance().getPerspective());
    setMat4(shader, "view", camera::instance().getView());

    setInt(shader, "texture1", 0);
    setInt(shader, "texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture.getID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shapeSpecularTexture.getID());

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, instanceCounts);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void shapeInstanced::drawShadow(const unsigned int& instanceCounts) const {

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, instanceCounts);
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------//

shapes& shapes::instance() {
    static shapes instance;
    return instance;
}

shapes::shapes(){

    const float vertices[] = {

        // Front
        -0.5, 0.5, 0.5,  0.0,0.0, 1.0, 0.0,0.0,
         0.5, 0.5, 0.5,  0.0,0.0, 1.0, 1.0,0.0,
        -0.5,-0.5, 0.5,  0.0,0.0, 1.0, 0.0,1.0,
         0.5,-0.5, 0.5,  0.0,0.0, 1.0, 1.0,1.0,
         
        // Back
         0.5, 0.5,-0.5,  0.0,0.0,-1.0, 0.0,0.0,
        -0.5, 0.5,-0.5,  0.0,0.0,-1.0, 1.0,0.0,
         0.5,-0.5,-0.5,  0.0,0.0,-1.0, 0.0,1.0,
        -0.5,-0.5,-0.5,  0.0,0.0,-1.0, 1.0,1.0,

        // Left
        -0.5, 0.5,-0.5,  -1.0,0.0,0.0, 0.0,0.0,
        -0.5, 0.5, 0.5,  -1.0,0.0,0.0, 1.0,0.0,
        -0.5,-0.5,-0.5,  -1.0,0.0,0.0, 0.0,1.0,
        -0.5,-0.5, 0.5,  -1.0,0.0,0.0, 1.0,1.0,

        // Right
         0.5, 0.5, 0.5,   1.0,0.0,0.0, 0.0,0.0,
         0.5, 0.5,-0.5,   1.0,0.0,0.0, 1.0,0.0,
         0.5,-0.5, 0.5,   1.0,0.0,0.0, 0.0,1.0,
         0.5,-0.5,-0.5,   1.0,0.0,0.0, 1.0,1.0,

        // Top
        -0.5, 0.5,-0.5,  0.0, 1.0,0.0, 0.0,0.0,
         0.5, 0.5,-0.5,  0.0, 1.0,0.0, 1.0,0.0,
        -0.5, 0.5, 0.5,  0.0, 1.0,0.0, 0.0,1.0,
         0.5, 0.5, 0.5,  0.0, 1.0,0.0, 1.0,1.0,

        // Bottom
         0.5,-0.5,-0.5,  0.0,-1.0,0.0, 0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,-1.0,0.0, 1.0,0.0,
         0.5,-0.5, 0.5,  0.0,-1.0,0.0, 0.0,1.0,
        -0.5,-0.5, 0.5,  0.0,-1.0,0.0, 1.0,1.0
    };

    unsigned int indices[] = {

        // Front
        0,1,2,
        1,3,2,

        // Back
        4,5,6,
        5,7,6,

        // Left
        8,9,10,
        9,11,10,

        // Right
        12,13,14,
        13,15,14,

        // Top
        16,17,18,
        17,19,18,

        // Bottom
        20,21,22,
        21,23,22
    };

    cube.bindVertices(vertices, sizeof(vertices), indices, sizeof(indices));
    cube.loadTexture(
        "C:/Users/sumit/Documents/GitHub/OpenGLRenderer/assets/textures/wood_box.png",
        "C:/Users/sumit/Documents/GitHub/OpenGLRenderer/assets/textures/metal_frame.png"
    );

    // Instanced
    cubeInstanced.bindVertices(vertices, sizeof(vertices), indices, sizeof(indices));
    cubeInstanced.loadTexture(
        "C:/Users/sumit/Documents/GitHub/OpenGLRenderer/assets/textures/wood_box.png",
        "C:/Users/sumit/Documents/GitHub/OpenGLRenderer/assets/textures/metal_frame.png"
    );

    float vertices2[] = {

        // Front
        -0.5f, 0.5f, 0.0f,0.0f,
         0.5f, 0.5f, 1.0f,0.0f,
        -0.5f,-0.5f, 0.0f,1.0f,
         0.5f,-0.5f, 1.0f,1.0f
    };

    unsigned int indices2[] = {

        // Front
        0,1,2,
        1,3,2
    };

    square.bindVertices(vertices2, sizeof(vertices2), indices2, sizeof(indices2));
    square.loadTexture(
        "C:/Users/sumit/Documents/GitHub/OpenGLRenderer/assets/textures/window_tint.png"
    );
}

//-------------------------------------------------------------------------------------//

cubeMap::cubeMap(const std::vector <std::string>& textureFaces) {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    unsigned char* pixelData;
    int width, height, nrChannels;

    const unsigned int size = textureFaces.size();

    for (unsigned int i = 0; i < size; i++) {

        pixelData = stbi_load(
            textureFaces[i].c_str(),
            &width, &height,
            &nrChannels, 0
        );

        if (pixelData) {

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height,
                0, GL_RGB, GL_UNSIGNED_BYTE, pixelData
            );
        }
        else {
            std::cerr << "ERROR :: FAILED TO LOAD CUBEMAP!" << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);

    const float skyboxVertices[] = {
                  
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

cubeMap::~cubeMap() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &textureID);
}