//
// Created by vaige on 27.2.2024.
//

#include "ResourceManager.h"
#include <stdexcept>


#ifndef SRC_DIR
#define SRC_DIR "."
#endif

static const std::string assetsDir = SRC_DIR "/assets";
static const std::string shadersDir = SRC_DIR "/shaders";


std::string texturePath(const std::string& textureName)
{
    return assetsDir + "/textures/" + textureName;
}

std::string modelPath(const std::string& modelName)
{
    return assetsDir + "/textures/" + modelName;
}

std::string shaderPath(const std::string& shaderName)
{
    return shadersDir + shaderName;
}


void ResourceManager::loadAll()
{
    loadTextures();
    loadModels();
    loadShaders();
}

Texture &ResourceManager::getTexture(const string &name)
{
    if (!mTextures.contains(name)) {
        mTextures[name] = Texture::loadFromFile(texturePath(name));
    }
    return mTextures[name];
}

Model &ResourceManager::getModel(const string &name)
{
    mModels.try_emplace(name, modelPath(name));
    return mModels.at(name);
}

Shader &ResourceManager::getShader(const string &name)
{
    return mShaders.at(name);
}

void ResourceManager::loadTextures()
{
    mTextures["bathroom-tiling"] = Texture::loadFromFile(texturePath("bathroom-tiling.jpg"));
}

void ResourceManager::loadModels()
{
    mModels.emplace("ak47", modelPath("ak47/ak47.obj"));
    mModels.emplace("backpack", modelPath("Backpack/backpack.obj"));
}

void ResourceManager::loadShaders()
{
    mShaders.insert({"model", Shader{shaderPath("model.vert").c_str(), shaderPath("model.frag").c_str()}});
    mShaders.insert({"color", Shader{shaderPath("model.vert").c_str(), shaderPath("color.frag").c_str()}});
}











