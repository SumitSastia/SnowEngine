#include <assetManager.h>
#include <debug.h>
#include <model.h>
#include <debug/assert.h>

#include <iostream>

uint32_t AssetManager::loadedTextures = 0;

AssetManager::Queue AssetManager::loadingQueue {};
std::vector <Texture> AssetManager::textures (MAX_TEXTURES);

std::unordered_map <std::string, TextureHandle> AssetManager::textureLookup {};
std::unordered_map <uint32_t, TextureHandle>    AssetManager::textureLookup_flat {};

void AssetManager::init() {

    loadingQueue.queue_size = 5;
    loadingQueue.init();
    
    loadTexture("assets/textures/error_texture.png");
}

const TextureHandle AssetManager::loadTexture_flatColor(const glm::vec4& color, const bool format) {

    const uint32_t hex = gfx::helper::rgba_to_hex(color);

    const auto lookupID = textureLookup_flat.find(hex);
    if (lookupID != textureLookup_flat.end()) return lookupID->second;

    SNOW_ASSERT(loadedTextures < MAX_TEXTURES, "MAX_TEXTURES ALLOCATION REACHED!");

    const TextureHandle handle = loadedTextures++;

    textures[handle].load(hex, format);
    textureLookup_flat[hex] = handle;

    std::cout << "Color Loaded: " << hex << ", Handle: " << handle << ", GL ID: " << textures[handle].getID() << '\n';
    return handle;
}

const TextureHandle AssetManager::loadTexture(const std::string& path, const bool format) {

    // Index Formatting
    const std::size_t start = path.find_last_of('/') + 1;
    const std::size_t end   = path.find('.');

    const std::string index = path.substr(start, end - start);

    const auto lookupID = textureLookup.find(index);
    if (lookupID != textureLookup.end()) return lookupID->second;

    SNOW_ASSERT(loadedTextures < MAX_TEXTURES, "MAX_TEXTURES ALLOCATION REACHED!");

    const TextureHandle handle = loadedTextures++;
    textureLookup[index] = handle;

    textures[handle].load(path, !format);
    // textures[handle].compile();

    std::cout << "Texture Loaded: " << std::hex << index << ", Handle: " << handle << ", GL ID: " << textures[handle].getID() << '\n';
    return handle;
}

void AssetManager::compileTextures() {

    for (uint32_t i = 0; i < loadedTextures; i++) {
        textures[i].compile();
    }
}

void AssetManager::destroyTexture(const TextureHandle& handle) {
    
    // textures[handle].destroy();
}

// Model Loading

ModelComponent AssetManager::loadModel(const std::string& path) {

    // Duplicate Prevention Implementation PENDING!
    return ModelComponent(path);
}

/////////////////////////////////////////////////////////////////////////////

const TextureHandle AssetManager::loadTex(const std::string& path, const bool format) {
    return 0;
}