//
// Created by vaige on 13.3.2024.
//

#ifndef FPSFROMSCRATCH_LOADINGSCREEN_H
#define FPSFROMSCRATCH_LOADINGSCREEN_H

#include "Mesh.hpp"
#include "Geometry.hpp"
#include "ResourceManager.hpp"

class LoadingScreen
{
public:
    explicit LoadingScreen(const std::string& image)
    : mMesh{Geometry::makePlane(1, 1, {ResourceManager::instance().getTexture(image)})}
    {
    }

    void draw(Shader& shader)
    {
        mMesh.draw(shader);
    }
private:
    Mesh mMesh;
};

#endif //FPSFROMSCRATCH_LOADINGSCREEN_H
