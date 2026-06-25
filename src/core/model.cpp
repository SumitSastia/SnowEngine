#include <model.h>
#include <renderer.h>
#include <assetManager.h>

#include <iostream>
#include <stb_image.h>

void MeshTexture::loadTexture(const char* path, const std::string& directory) {

    int width, height;
    checkPath = directory + "/" + path;

    std::string base_str = base.string();

    base_str.erase(base_str.size() - 6);
    checkPath.erase(0, 2);

    std::string finalPath = base_str + checkPath;

    unsigned char* pixelData = stbi_load(finalPath.c_str(), &width, &height, nullptr, 4);

    if (!pixelData) {
        std::cerr << "Failed to Load Image!\n" << finalPath << std::endl;
        return;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//-------------------------------------------------------------------------------------//

void Mesh::setupMesh() {

    size_t size_v = sizeof(Vertex) * vertices.size();
    size_t size_i = sizeof(unsigned int) * indices.size();

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_v, vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCords));
    glEnableVertexAttribArray(2);
}

void Mesh::bindTextures(const unsigned int textureUnit) const {

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textures[0].id);
}

void Mesh::draw() const {

    const bool cullingState = Renderer::getCullingState();

    Renderer::disableCulling();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (cullingState) Renderer::enableCulling();
}

void Mesh::destroy() {

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

//-------------------------------------------------------------------------------------//

void Model3D::loadModel(const std::string& path) {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "ERROR::ASSIMP::Unable to Open the Model File!" << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);

    total_mesh = meshes.size();
}

void Model3D::processNode(aiNode* node, const aiScene* scene) {

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {

        aiMesh* Mesh_part = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(Mesh_part, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model3D::processMesh(aiMesh* _Mesh, const aiScene* scene) {

    std::vector <Vertex> vertices;
    std::vector <unsigned int> indices;
    std::vector <MeshTexture> textures;

    for (unsigned int i = 0; i < _Mesh->mNumVertices; i++) {

        Vertex temp_Vertex{};

        temp_Vertex.position = glm::vec3(
            _Mesh->mVertices[i].x,
            _Mesh->mVertices[i].y,
            _Mesh->mVertices[i].z
        );

        // Frustum Culling - Data
        float vertex_length = glm::length(temp_Vertex.position);
        radius = std::max(radius, vertex_length);

        minCorner = glm::min(minCorner, temp_Vertex.position);
        maxCorner = glm::max(maxCorner, temp_Vertex.position);

        if (_Mesh->HasNormals()) {

            temp_Vertex.normal = glm::vec3(
                _Mesh->mNormals[i].x,
                _Mesh->mNormals[i].y,
                _Mesh->mNormals[i].z
            );
        }

        if (_Mesh->mTextureCoords[0]) {

            temp_Vertex.textureCords = glm::vec2(
                _Mesh->mTextureCoords[0][i].x,
                _Mesh->mTextureCoords[0][i].y
            );
        }
        else {
            temp_Vertex.textureCords = glm::vec2(0.0f);
        }

        vertices.push_back(temp_Vertex);
    }

    for (unsigned int i = 0; i < _Mesh->mNumFaces; i++) {

        aiFace face = _Mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {

            indices.push_back(face.mIndices[j]);
        }
    }

    if (_Mesh->mMaterialIndex >= 0) {

        aiMaterial* material = scene->mMaterials[_Mesh->mMaterialIndex];

        std::vector <MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector <MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<MeshTexture> Model3D::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& type_str) const {

    std::vector <MeshTexture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {

        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for (const MeshTexture& tex : loadedTextures) {

            if (std::strcmp(tex.checkPath.data(), str.C_Str()) == 0) {

                textures.push_back(tex);
                skip = true; break;
            }
        }

        if (!skip) {

            MeshTexture tex;
            tex.loadTexture(str.C_Str(), directory);
            tex.type = type_str;
            textures.push_back(tex);
        }
    }
    return textures;
}

void Model3D::draw() const {

    for (unsigned int i = 0; i < total_mesh; i++) {
        meshes[i].draw();
    }
}

void Model3D::bindTextures(const unsigned int firstTextureUnit) const {

    for (unsigned int i = 0; i < total_mesh; i++) {
        meshes[i].bindTextures(firstTextureUnit + i);
    }
}

void Model3D::destroy() {

    for (unsigned int i = 0; i < total_mesh; i++) {
        meshes[i].destroy();
    }
}

void Mesh::clean() {

    vertices.clear();
    indices.clear();
    textures.clear();
}

float Model3D::clean() {

    for (unsigned int i = 0; i < total_mesh; i++) {
        meshes[i].clean();
    }

    meshes.clear();
    loadedTextures.clear();

    return radius;
}

//////////////////////////////////////////////////////////////////////////////////////////////

ModelComponent::ModelComponent(const std::string& path) {

    const std::string finalPath = "../" + path;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(finalPath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "ERROR::ASSIMP::Unable to Open the Model File!" << std::endl;
        return;
    }

    // const std::size_t start = path.find_first_of('/') + 1;
    const std::size_t end   = path.find_last_of('/');

    processNode(scene->mRootNode, scene, path.substr(0, end - 0));
}

TextureHandle ModelComponent::loadMaterialTexture(aiMaterial* material, aiTextureType type, const std::string& path) {

    aiString str;
    material->GetTexture(type, 0, &str);

    std::string finalPath = path + "/" + str.C_Str();

    return AssetManager::loadTexture(finalPath, 1);
}

void ModelComponent::processNode(aiNode* node, const aiScene* scene, const std::string& path) {

    for (uint i = 0; i < node->mNumMeshes; i++) {

        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, path));
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, path);
    }
}

MeshComponent ModelComponent::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path) {

    std::vector <Vertex> vertices;
    std::vector <uint>   indices;

    for (uint i = 0; i < mesh->mNumVertices; i++) {

        Vertex vertex {};

        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }

        if (mesh->mTextureCoords[0]) {

            vertex.textureCords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else {
            vertex.textureCords = glm::vec2(0.5f);
        }

        // Frustum Culling - Data
        radius = std::max(radius, glm::length(vertex.position));

        minCorner = glm::min(minCorner, vertex.position);
        maxCorner = glm::max(maxCorner, vertex.position);

        vertices.push_back(vertex);
    }

    for (uint i = 0; i < mesh->mNumFaces; i++) {
        
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++) {

            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {

        aiColor3D color;
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0){
            textures.push_back(loadMaterialTexture(material, aiTextureType_DIFFUSE, path));
        }
        else if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            
            glm::vec4 flatColor = {
                color.r,
                color.g,
                color.b,
                1.0f
            };

            // glm::vec4 flatColor = {
            //     std::pow(color.r, 1.6f),
            //     std::pow(color.g, 1.6f),
            //     std::pow(color.b, 1.6f),
            //     1.0f
            // };

            textures.push_back(AssetManager::loadTexture_flatColor(flatColor));
        }
        else {
            textures.push_back(0);
        }

    }
    else {
        // If mesh has no texture
        textures.push_back(0);
    }

    MeshComponent meshComponent;
    meshComponent.loadMesh3D(vertices, indices);

    return meshComponent;
}