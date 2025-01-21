//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_RESOURCEMANAGER_H
#define FPSFROMSCRATCH_RESOURCEMANAGER_H

#include <memory>
#include <thread>
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
        std::thread t{[this]() {
            std::lock_guard g{mtx};
            prepareAll();
            prepared = true;
        }};
        t.detach();
    }

    void shutdown() {
        mTextures.clear();
        mModels.clear();
    }

    void loadAll();

    bool isPrepared() {
        return prepared;
    }

    Texture& getTexture(const std::string& name);
    Model& getModel(const std::string& name);
    std::unique_ptr<SkinnedModel> getSkinnedModel(const std::string& name);
    const std::map<std::string, std::unique_ptr<Animation>>& getAnimations() const;
    const std::unique_ptr<Animation>& getAnimation(const std::string& name) const;

private:
    void prepareAll();
    void prepareModels();
    void prepareTextures();
    void loadAnimations();

    std::unordered_map<std::string, Texture> mTextures;
    std::unordered_map<std::string, Model> mModels;
    std::map<std::string, std::unique_ptr<Animation>> mAnimations;

    std::mutex mtx;
    std::atomic<bool> prepared{false};
};

static ResourceManager& gResources = ResourceManager::instance();

#endif //FPSFROMSCRATCH_RESOURCEMANAGER_H
