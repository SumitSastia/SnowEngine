#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <shader.h>

#define MAX_TEXTURES 100
using TextureHandle = uint32_t;

class AssetManager {

    static uint32_t total_loadedTextures;

    static std::unordered_map <std::string, TextureHandle> textureLookup;
    
public:
    static std::vector <Texture2D> textures;

    static TextureHandle loadTexture(const std::string& path, const bool format = 0);
    
    static const Texture2D& getTexture(const TextureHandle& handle) { return textures[handle]; }

    static void destroyTexture(const TextureHandle& handle);
};