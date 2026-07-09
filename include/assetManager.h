#pragma once

#include <string>
#include <vector>
#include <thread>
#include <unordered_map>

#include <ecs/component.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <texture.h>

using TextureHandle = uint32_t;

class AssetManager {

    struct MetaData {
        
        TextureHandle handle;
        std::string path;
        bool format;
    };

    struct Queue {

        uint8_t counter    = 0;
        uint8_t queue_size = 0;
        MetaData* metaData = nullptr;

        void init() {
            metaData = new MetaData[queue_size];
        }

        void add(MetaData data) {
            metaData[counter] = data;

            if (++counter == queue_size) {
                load();
                compile();
            }
        }

        void load();
        void compile();
    };

    static Queue loadingQueue;

    static uint32_t loadedTextures;
    static std::vector <Texture> textures;

    static std::unordered_map <std::string, TextureHandle> textureLookup;
    static std::unordered_map <uint32_t, TextureHandle>    textureLookup_flat;
    
public:

    static void init();
    
    static const TextureHandle loadTexture_flatColor(const glm::vec4& color, const bool isRGBA = 1);
    static const TextureHandle loadTexture(const std::string& path, const bool isRGBA = 0);
    static const Texture&      getTexture(const TextureHandle& handle) { return textures[handle]; }

    static void compileTextures();
    
    static ModelComponent loadModel(const std::string& path, const bool normalMapped = false);
    
    static void destroyTexture(const TextureHandle& handle);

    // TESTING PHASE
    static const TextureHandle loadTex(const std::string& path, const bool format = 0);
};