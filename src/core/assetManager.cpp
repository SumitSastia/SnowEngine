#include <assetManager.h>
#include <debug.h>
#include <model.h>
#include <debug/assert.h>

#include <iostream>

uint32_t AssetManager::loadedTextures = 0;
uint32_t AssetManager::loadedModels   = 0;

AssetManager::Queue   AssetManager::loadingQueue {};

// std::vector <Model>   AssetManager::models   (MAX_MODELS);
std::vector <Texture> AssetManager::textures (MAX_TEXTURES);

std::unordered_map <std::string, TextureHandle> AssetManager::textureLookup {};
std::unordered_map <uint32_t, TextureHandle>    AssetManager::textureLookup_flat {};

void AssetManager::init() {

    loadingQueue.queue_size = 5;
    loadingQueue.init();
    
    loadTexture_flatColor(glm::vec4(1.0f));                   // 0 - Non-Existent
    loadTexture("assets/textures/error_texture.png");         // 1 - Albedo Default
    loadTexture_flatColor(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)); // 2 - Normal Default
}

const TextureHandle AssetManager::loadTexture_flatColor(const glm::vec4& color, const bool isRGBA) {

    const uint32_t hex = gfx::helper::rgba_to_hex(color);

    const auto lookupID = textureLookup_flat.find(hex);
    if (lookupID != textureLookup_flat.end()) return lookupID->second;

    SNOW_ASSERT(loadedTextures < MAX_TEXTURES, "MAX_TEXTURES ALLOCATION REACHED!");

    const TextureHandle handle = loadedTextures++;

    textures[handle].load(hex, isRGBA);
    textureLookup_flat[hex] = handle;

    // std::cout << "Color Loaded: " << std::hex << std::setw(8) << std::setfill('0') << hex
    //           << ", Handle: "     << std::dec << handle
    //         //   << ", GL ID: "      << textures[handle].getID()
    //           << '\n';

    DebugMenu::addAssetInitTime(0, hex, handle);

    return handle;
}

const TextureHandle AssetManager::loadTexture(const std::string& path, const bool isRGBA) {

    // Index Formatting
    const std::size_t start = path.find_last_of('/') + 1;
    const std::size_t end   = path.find('.');

    const std::string index = path.substr(start, end - start);

    const auto lookupID = textureLookup.find(index);
    if (lookupID != textureLookup.end()) return lookupID->second;

    SNOW_ASSERT(loadedTextures < MAX_TEXTURES, "MAX_TEXTURES ALLOCATION REACHED!");

    const TextureHandle handle = loadedTextures++;
    textureLookup[index] = handle;

    textures[handle].load(path, isRGBA);
    // textures[handle].compile();

    // std::cout << "Texture Loaded: " << std::left << std::setw(20) << std::setfill(' ') << index
    //           << ", Handle: "       << handle
    //         //   << ", GL ID: " << textures[handle].getID()
    //           << '\n';

    DebugMenu::addAssetInitTime(index, 0, handle);

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

ModelComponent AssetManager::loadModel(const std::string& path, const bool normalMapped) {

    // Duplicate Prevention Implementation PENDING!
    return ModelComponent(path, normalMapped);
}

/////////////////////////////////////////////////////////////////////////////

const TextureHandle AssetManager::loadTex(const std::string& path, const bool format) {
    return 0;
}