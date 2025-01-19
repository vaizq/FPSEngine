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

    const std::vector<Animation>& getAnimations() const {
        return mAnimations;
    }

    const Animation* getAnimation(const std::string& name) const {
        for (const auto& animation : mAnimations) {
            if (animation.name == name) {
                return &animation;
            }
        }
        printf("ERROR: unable to find animation %s\n", name.c_str());
        return nullptr;
    }

    void reloadShaders();

private:
    void loadAll();
    void loadTextures();
    void loadModels();
    void loadShaders();
    void loadAnimations();

    std::unordered_map<std::string, Texture> mTextures;
    std::unordered_map<std::string, Model> mModels;
    std::unordered_map<std::string, Shader> mShaders;
    std::vector<Animation> mAnimations;
};

static ResourceManager& gResources = ResourceManager::instance();


#endif //FPSFROMSCRATCH_RESOURCEMANAGER_H
