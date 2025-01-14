#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "engine/Singleton.hpp"

class AssetManager : public Singleton<AssetManager> {
public:
private:
};

static AssetManager& gAssetManager = AssetManager::instance();

#endif
