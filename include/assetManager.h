#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <shader.h>
#include <ecs/component.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define MAX_TEXTURES 100
using TextureHandle = uint32_t;

class AssetManager {

    static uint32_t total_loadedTextures;
    static std::vector <Texture2D> textures;

    static std::unordered_map <std::string, TextureHandle> textureLookup;
    
public:

    static void init();
    
    static TextureHandle    loadTexture(const std::string& path, const bool format = 0);
    static const Texture2D& getTexture(const TextureHandle& handle) { return textures[handle]; }
    
    static ModelComponent loadModel(const std::string& path);
    
    static void destroyTexture(const TextureHandle& handle);
};