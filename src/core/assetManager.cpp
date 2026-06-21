#include <assetManager.h>
#include <debug.h>
#include <model.h>

#include <iostream>

uint32_t AssetManager::total_loadedTextures = 0;

std::vector <Texture2D> AssetManager::textures (MAX_TEXTURES);
std::unordered_map <std::string, TextureHandle> AssetManager::textureLookup {};

void AssetManager::init() {
    loadTexture("assets/textures/error_texture.png");
}

TextureHandle AssetManager::loadTexture(const std::string& path, const bool format) {

    // Index Formatting
    const std::size_t start = path.find_last_of('/') + 1;
    const std::size_t end   = path.find('.');

    const std::string index = path.substr(start, end - start);

    const auto lookupID = textureLookup.find(index);
    if (lookupID != textureLookup.end()) return lookupID->second;

    if (total_loadedTextures == MAX_TEXTURES) {
        std::cerr << "ERROR::MAX_TEXTURES ALLOCATION REACHED!" << std::endl;
        return 0;
    }

    const TextureHandle handle = total_loadedTextures++;
    textures[handle].load(path.c_str(), format);

    textureLookup[index] = handle;

    std::cout << "Loaded: " << index << ", Handle: " << handle << ", GL ID: " << textures[handle].getID() << '\n';
    return handle;
}

void AssetManager::destroyTexture(const TextureHandle& textureID) {
    
}

// Model Loading

ModelComponent AssetManager::loadModel(const std::string& path) {

    return ModelComponent(path);
}