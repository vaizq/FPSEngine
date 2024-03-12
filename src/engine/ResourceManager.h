//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_RESOURCEMANAGER_H
#define FPSFROMSCRATCH_RESOURCEMANAGER_H

#include <memory>
#include "Texture.h"
#include "Model.h"
#include <string>
#include <unordered_map>


class ResourceManager
{
public:
    static ResourceManager& instance()
    {
        static std::unique_ptr<ResourceManager> self;
        if (self == nullptr) {
            self = std::unique_ptr<ResourceManager>(new ResourceManager);
        }
        return *self;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    void loadAll();

    Texture& getTexture(const std::string& name);
    Model& getModel(const std::string& name);
    Shader& getShader(const std::string& name);
    void reloadShaders();

    glm::mat4 view{1.0f};
    glm::mat4 projection{1.0f};

private:
    ResourceManager() = default;

    void loadTextures();
    void loadModels();
    void loadShaders();

    std::unordered_map<std::string, Texture> mTextures;
    std::unordered_map<std::string, Model> mModels;
    std::unordered_map<std::string, Shader> mShaders;
};


#endif //FPSFROMSCRATCH_RESOURCEMANAGER_H
