#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "engine/Singleton.hpp"
#include "Mesh.hpp"
#include "SkinnedModel.hpp"
#include "Skeleton.hpp"


class AssetManager : public Singleton<AssetManager> {
public:
private:
    SkinnedModel loadSkinnedModel(const char* path);
};

static AssetManager& gAssetManager = AssetManager::instance();

#endif
