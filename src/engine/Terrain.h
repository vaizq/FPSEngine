//
// Created by vaige on 13.3.2024.
//

#ifndef FPSFROMSCRATCH_TERRAIN_H
#define FPSFROMSCRATCH_TERRAIN_H

#include "GameObject.h"
#include "Perlin.h"


class Terrain : public GameObject
{
public:
    Terrain();
    ~Terrain() override;
    void onGUI() override;
    float height(const glm::vec3& pos);
    void render(Shader& shader, const glm::mat4& parentTransform) override;
private:
    void loadTerrain(glm::vec3 scale);
    Perlin2D mPerlin;
    glm::vec3 mTerrainScale{1.0f};
};


#endif //FPSFROMSCRATCH_TERRAIN_H
