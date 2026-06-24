#pragma once

#include <string>
#include <vector>
#include <thread>
#include <unordered_map>

#include <shader.h>
#include <ecs/component.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <texture.h>

#define MAX_TEXTURES 100
using TextureHandle = uint32_t;

class AssetManager {

    static uint32_t total_loadedTextures;
    static std::vector <Texture> textures;

    static std::unordered_map <std::string, TextureHandle> textureLookup;
    static std::unordered_map <uint32_t, TextureHandle>    textureLookup_flat;
    
public:

    static void init();
    
    static const TextureHandle loadTexture_flatColor(const glm::vec4& color);
    static const TextureHandle loadTexture(const std::string& path, const bool format = 0);
    static const Texture&      getTexture(const TextureHandle& handle) { return textures[handle]; }

    static void compileTextures();
    
    static ModelComponent loadModel(const std::string& path);
    
    static void destroyTexture(const TextureHandle& handle);
};