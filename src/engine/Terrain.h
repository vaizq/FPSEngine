//
// Created by vaige on 13.3.2024.
//

#ifndef FPSFROMSCRATCH_TERRAIN_H
#define FPSFROMSCRATCH_TERRAIN_H

#include "GameObject.h"


class Terrain : public GameObject
{
public:
    Terrain();
    ~Terrain();
    float height(const glm::vec3& pos);
    void render(Shader& shader, const glm::mat4& parentTransform) override;
};


#endif //FPSFROMSCRATCH_TERRAIN_H
