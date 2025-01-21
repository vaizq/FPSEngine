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
    }

    Texture& getTexture(const std::string& name);
    Model& getModel(const std::string& name);
    std::unique_ptr<SkinnedModel> getSkinnedModel(const std::string& name);

    const std::map<std::string, std::unique_ptr<Animation>>& getAnimations() const {
        return mAnimations;
    }

    const std::unique_ptr<Animation>& getAnimation(const std::string& name) const {
        return mAnimations.at(name);
    }

private:
    void loadAll();
    void loadTextures();
    void loadModels();
    void loadAnimations();

    std::unordered_map<std::string, Texture> mTextures;
    std::unordered_map<std::string, Model> mModels;
    std::map<std::string, std::unique_ptr<Animation>> mAnimations;
};

static ResourceManager& gResources = ResourceManager::instance();

#endif //FPSFROMSCRATCH_RESOURCEMANAGER_H
