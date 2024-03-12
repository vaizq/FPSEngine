//
// Created by vaige on 12.3.2024.
//

#ifndef FPSFROMSCRATCH_LIGHT_H
#define FPSFROMSCRATCH_LIGHT_H

#include "GameObject.h"
#include "ResourceManager.h"
#include "Geometry.h"

struct Light : public GameObject
{
    Transform transform{};
    Mesh mesh{Geometry::makeBox()};

    void render(Shader& shader) override
    {
        auto& colorShader = ResourceManager::instance().getShader("color");
        colorShader.setMat4("view", ResourceManager::instance().view);
        colorShader.setMat4("projection", ResourceManager::instance().projection);
        colorShader.setMat4("model", transform.modelMatrix());

    }
};


#endif //FPSFROMSCRATCH_LIGHT_H
