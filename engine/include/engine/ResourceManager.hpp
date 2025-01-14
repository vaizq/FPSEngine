//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_RESOURCEMANAGER_H
#define FPSFROMSCRATCH_RESOURCEMANAGER_H

#include <memory>
#include "Texture.hpp"
#include "Model.hpp"
#include <string>
#include <unordered_map>
#include "engine/Singleton.hpp"
#include "SkinnedModel.hpp"


class ResourceManager : public Singleton<ResourceManager>
{
public:
    void startup() {
        loadAll();
    }

    void shutdown() {
        mTextures.clear();
        mModels.clear();
        mShaders.clear();
    }

    Texture& getTexture(const std::string& name);
    Model& getModel(const std::string& name);
    std::unique_ptr<SkinnedModel> getSkinnedModel(const std::string& name);

    Shader& getShader(const std::string& name);
    void reloadShaders();

private:
    void loadAll();
    void loadTextures();
    void loadModels();
    void loadShaders();

    std::unordered_map<std::string, Texture> mTextures;
    std::unordered_map<std::string, Model> mModels;
    std::unordered_map<std::string, Shader> mShaders;
};

static ResourceManager& gResources = ResourceManager::instance();


#endif //FPSFROMSCRATCH_RESOURCEMANAGER_H
