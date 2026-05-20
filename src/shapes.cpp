#include <shapes.h>
#include <camera.h>
#include <shader.h>

#include <iostream>
#include <stb_image.h>

//-------------------------------------------------------------------------------------//

/*
Allocates vertices & indices into Memory.
Initializes the VBO, VAO, EBO
*/
void Shape::bindVertices2D(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(u_int);

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/*
Allocates vertices & indices into Memory.
Initializes the VBO, VAO, EBO
*/
void Shape::bindVertices3D(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(u_int);

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Shape::bindVertices3D_Mapped(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i
) {
    indicesCount = size_i / sizeof(u_int);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TextureCords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Shape::bindDiffuseTex(const unsigned int textureUnit) const {

    if (!shapeDiffuseTexture) {
        std::cerr << "Shape DiffuseTexture not Loaded!" << std::endl;
    }
    else {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, shapeDiffuseTexture->getID());
    }
}

void Shape::bindNormalTex(const unsigned int textureUnit) const {

    if (!shapeNormalTexture) {
        std::cerr << "Shape NormalTexture not Loaded!" << std::endl;
    }
    else {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, shapeNormalTexture->getID());
    }
}

void Shape::bindSpecularTex(const unsigned int textureUnit) const {

    if (!shapeSpecularTexture) {
        std::cerr << "Shape SpecularTexture not Loaded!" << std::endl;
    }
    else {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, shapeSpecularTexture->getID());
    }
}

void Shape::loadDiffuseTex(const char* path) {

    shapeDiffuseTexture = (!shapeDiffuseTexture)? new Texture2D() : shapeDiffuseTexture;
    shapeDiffuseTexture->load(path);
}

void Shape::loadNormalTex(const char* path) {

    shapeNormalTexture = (!shapeNormalTexture)? new Texture2D() : shapeNormalTexture;
    shapeNormalTexture->load(path);
}

void Shape::loadSpecularTex(const char* path) {

    shapeSpecularTexture = (!shapeSpecularTexture)? new Texture2D() : shapeSpecularTexture;
    shapeSpecularTexture->load(path);
}

void Shape::draw() const {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
}


//-------------------------------------------------------------------------------------//

void ShapeInstanced::bindVertices(
    const float* vertices, const size_t& size_v,
    const unsigned int* indices, const size_t& size_i,
    const glm::mat4* models, const size_t& size_m,
    const glm::mat3* normals, const size_t& size_n
) {

    indicesCount = size_i / sizeof(u_int);
    instanceCounts = size_m / sizeof(glm::mat4);

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
    
    // InstancedModel
    glGenBuffers(1, &instanceModelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceModelVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceCounts * sizeof(glm::mat4), models, GL_STATIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);
    glBindBuffer(GL_ARRAY_BUFFER, instanceModelVBO);
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glVertexAttribDivisor(3, 1);
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glVertexAttribDivisor(4, 1);
    
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glVertexAttribDivisor(5, 1);
    
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
    glVertexAttribDivisor(6, 1);
    
    // InstancedNormal
    glGenBuffers(1, &instanceNormalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceNormalVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceCounts * sizeof(glm::mat3), normals, GL_STATIC_DRAW);

    std::size_t vec3Size = sizeof(glm::vec3);
    glBindBuffer(GL_ARRAY_BUFFER, instanceNormalVBO);
    
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)0);
    glVertexAttribDivisor(7, 1);
    
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)(1 * vec3Size));
    glVertexAttribDivisor(8, 1);
    
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, 3 * vec3Size, (void*)(2 * vec3Size));
    glVertexAttribDivisor(9, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ShapeInstanced::draw() const {

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr, instanceCounts);
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------//

DefaultShapes& DefaultShapes::instance() {
    static DefaultShapes instance {};
    return instance;
}

DefaultShapes::DefaultShapes(){

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

    const float vertices1[] = {

        // Front         // Normals     // UV     // Tangents
        -0.5, 0.5, 0.5,  0.0,0.0, 1.0,  0.0,0.0,  1.0,0.0,0.0,
         0.5, 0.5, 0.5,  0.0,0.0, 1.0,  1.0,0.0,  1.0,0.0,0.0,
        -0.5,-0.5, 0.5,  0.0,0.0, 1.0,  0.0,1.0,  1.0,0.0,0.0,
         0.5,-0.5, 0.5,  0.0,0.0, 1.0,  1.0,1.0,  1.0,0.0,0.0,
         
        // Back
         0.5, 0.5,-0.5,  0.0,0.0,-1.0,  0.0,0.0, -1.0,0.0,0.0,
        -0.5, 0.5,-0.5,  0.0,0.0,-1.0,  1.0,0.0, -1.0,0.0,0.0,
         0.5,-0.5,-0.5,  0.0,0.0,-1.0,  0.0,1.0, -1.0,0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,0.0,-1.0,  1.0,1.0, -1.0,0.0,0.0,

        // Left
        -0.5, 0.5,-0.5,  -1.0,0.0,0.0,  0.0,0.0,  0.0,0.0,1.0,
        -0.5, 0.5, 0.5,  -1.0,0.0,0.0,  1.0,0.0,  0.0,0.0,1.0,
        -0.5,-0.5,-0.5,  -1.0,0.0,0.0,  0.0,1.0,  0.0,0.0,1.0,
        -0.5,-0.5, 0.5,  -1.0,0.0,0.0,  1.0,1.0,  0.0,0.0,1.0,

        // Right
         0.5, 0.5, 0.5,  1.0,0.0,0.0,   0.0,0.0, 0.0,0.0,-1.0,
         0.5, 0.5,-0.5,  1.0,0.0,0.0,   1.0,0.0, 0.0,0.0,-1.0,
         0.5,-0.5, 0.5,  1.0,0.0,0.0,   0.0,1.0, 0.0,0.0,-1.0,
         0.5,-0.5,-0.5,  1.0,0.0,0.0,   1.0,1.0, 0.0,0.0,-1.0,

        // Top
        -0.5, 0.5,-0.5,  0.0, 1.0,0.0,  0.0,0.0,  1.0,0.0,0.0,
         0.5, 0.5,-0.5,  0.0, 1.0,0.0,  1.0,0.0,  1.0,0.0,0.0,
        -0.5, 0.5, 0.5,  0.0, 1.0,0.0,  0.0,1.0,  1.0,0.0,0.0,
         0.5, 0.5, 0.5,  0.0, 1.0,0.0,  1.0,1.0,  1.0,0.0,0.0,

        // Bottom
         0.5,-0.5,-0.5,  0.0,-1.0,0.0,  0.0,0.0,  1.0,0.0,0.0,
        -0.5,-0.5,-0.5,  0.0,-1.0,0.0,  1.0,0.0,  1.0,0.0,0.0,
         0.5,-0.5, 0.5,  0.0,-1.0,0.0,  0.0,1.0,  1.0,0.0,0.0,
        -0.5,-0.5, 0.5,  0.0,-1.0,0.0,  1.0,1.0,  1.0,0.0,0.0
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

    cube.bindVertices3D(vertices, sizeof(vertices), indices, sizeof(indices));
    cube.loadDiffuseTex("assets/textures/wood_box.png");

    advancedCube.bindVertices3D_Mapped(vertices1, sizeof(vertices1), indices, sizeof(indices));
    advancedCube.loadDiffuseTex("assets/textures/wood_box.png");
    advancedCube.loadSpecularTex("assets/textures/metal_frame.png");
    advancedCube.loadNormalTex("assets/textures/wood_box_normal.png");

    // Instanced / Multiple Cubes
    unsigned int totalCubes = 10;
    glm::vec3 cubePositions[totalCubes] = {

        glm::vec3(5.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    glm::mat4 objModels[10];
    glm::mat3 objNormals[10];

    for (unsigned int i = 0; i < totalCubes; i++) {

        glm::mat4 objModel = glm::translate(glm::mat4(1.0f), cubePositions[i]);

        objModels[i] = glm::rotate(objModel, i * 15.0f, glm::vec3(1.0f, 2.0f, 3.0f));
        objNormals[i] = glm::transpose(glm::inverse(glm::mat3(objModels[i])));
    }

    cubeInstanced.bindVertices(
        vertices,   sizeof(vertices), 
        indices,    sizeof(indices), 
        objModels,  sizeof(objModels), 
        objNormals, sizeof(objNormals)
    );

    cubeInstanced.loadDiffuseTex("assets/textures/wood_box.png");

    // Plane

    float vertices2[] = {

        -0.5f, 0.5f, 0.0f,0.0f,
         0.5f, 0.5f, 1.0f,0.0f,
        -0.5f,-0.5f, 0.0f,1.0f,
         0.5f,-0.5f, 1.0f,1.0f
    };

    unsigned int indices2[] = {

        0,1,2,
        1,3,2
    };

    square.bindVertices2D(vertices2, sizeof(vertices2), indices2, sizeof(indices2));
    square.loadDiffuseTex("assets/textures/brickwall.jpg");
    square.loadNormalTex("assets/textures/brickwall_normal.png");
}

//-------------------------------------------------------------------------------------//