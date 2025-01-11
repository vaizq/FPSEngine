//
// Created by vaige on 27.2.2024.
//

#include "ResourceManager.hpp"
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
    return assetsDir + "/models/" + modelName;
}

std::string shaderPath(const std::string& shaderName)
{
    return shadersDir + '/' + shaderName;
}

void ResourceManager::loadAll()
{
    loadTextures();
    loadModels();
    loadShaders();
}

Texture &ResourceManager::getTexture(const string &name)
{
    return mTextures.at(name);
}

Model &ResourceManager::getModel(const string &name)
{
    return mModels.at(name);
}

Shader &ResourceManager::getShader(const string &name)
{
    return mShaders.at(name);
}

void ResourceManager::loadTextures()
{
    mTextures["bathroom-tiling"] = Texture::loadFromFile(texturePath("bathroom-tiling.jpg"));
    mTextures["dirt"] = Texture::loadFromFile(texturePath("dirt.jpg"));
    mTextures["loadingScreen"] = Texture::loadFromFile(texturePath("halloween.jpg"));
}

void ResourceManager::loadModels()
{
    mModels.emplace("soldier", modelPath("Soldier/soldier.fbx"));
    mModels.emplace("monster", modelPath("Monster/Monster.dae"));
}

void ResourceManager::loadShaders()
{
    mShaders.insert({"model", Shader{shaderPath("model.vert").c_str(), shaderPath("model.frag").c_str()}});
    mShaders.insert({"color", Shader{shaderPath("color.vert").c_str(), shaderPath("color.frag").c_str()}});
}

void ResourceManager::reloadShaders()
{
    for (auto& [name, shader] : mShaders) {
        shader.reload();
    }
}
