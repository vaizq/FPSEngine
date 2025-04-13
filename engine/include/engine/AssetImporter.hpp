#ifndef ASSET_IMPORTER_HPP
#define ASSET_IMPORTER_HPP

#include "Skeleton.hpp"
#include "Animation.hpp"
#include "Mesh.hpp"
#include <vector>
#include <assimp/scene.h>


namespace AssetImporter {
    std::vector<Animation> loadAnimations(const aiScene* scene);
    Skeleton loadSkeleton(const aiScene* scene);
    std::vector<Mesh> loadMeshes(const aiScene* scene, const Skeleton& skeleton, const std::string& directory);
}



#endif
