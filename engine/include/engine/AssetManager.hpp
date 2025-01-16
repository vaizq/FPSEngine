#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "engine/Singleton.hpp"
#include "Mesh.hpp"
#include "SkinnedModel.hpp"
#include "Skeleton.hpp"


class AssetManager : public Singleton<AssetManager> {
public:
    std::vector<Animation> loadAnimations(const aiScene* scene);
};

static AssetManager& gAssetManager = AssetManager::instance();

#endif
