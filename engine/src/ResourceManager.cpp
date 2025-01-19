//
// Created by vaige on 27.2.2024.
//

#include "ResourceManager.hpp"
#include "AssetImporter.hpp"
#include <filesystem>


#ifndef SRC_DIR
#define SRC_DIR "."
#endif

static const std::string assetsDir = SRC_DIR "/assets";
static const std::string animationDir = SRC_DIR "/assets/animations";


std::string texturePath(const std::string& textureName)
{
    return assetsDir + "/textures/" + textureName;
}

std::string modelPath(const std::string& modelName)
{
    return assetsDir + "/models/" + modelName;
}

void ResourceManager::loadAll()
{
    loadTextures();
    loadModels();
    loadAnimations();
}

Texture &ResourceManager::getTexture(const string &name)
{
    return mTextures.at(name);
}

Model &ResourceManager::getModel(const string &name)
{
    return mModels.at(name);
}

std::unique_ptr<SkinnedModel> ResourceManager::getSkinnedModel(const string &name)
{
    Model& model = mModels.at(name);
    return std::make_unique<SkinnedModel>(model.skeleton, model);
}

void ResourceManager::loadTextures()
{
    mTextures["bathroom-tiling"] = Texture::loadFromFile(texturePath("bathroom-tiling.jpg"));
    mTextures["dirt"] = Texture::loadFromFile(texturePath("dirt.jpg"));
    mTextures["loadingScreen"] = Texture::loadFromFile(texturePath("halloween.jpg"));
}

void ResourceManager::loadModels()
{
    mModels.emplace("soldier", modelPath("Soldier/Soldier.fbx"));
    mModels.emplace("nurse", modelPath("Nurse/Nurse.fbx"));
}

std::vector<Animation> loadAnimationsFrom(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
                                             aiProcess_LimitBoneWeights |
                                             aiProcess_Triangulate | 
                                             aiProcess_GenSmoothNormals | 
                                             aiProcess_FlipUVs |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_PopulateArmatureData |
                                             aiProcess_ValidateDataStructure |
                                             aiProcess_FindInvalidData);

    // check for errors
    if(!scene) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return {};
    }

    return AssetImporter::loadAnimations(scene);
}

void ResourceManager::loadAnimations()
{
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator{animationDir}) {
        if (dirEntry.path().extension() == ".fbx") {
            const auto& path = dirEntry.path();
            auto animations = loadAnimationsFrom(path.c_str());
            if (!animations.empty()) {
                animations.front().name = path.filename().c_str();
                mAnimations.push_back(animations.front());
            }
        }
    }
}
