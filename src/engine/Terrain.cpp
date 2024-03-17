//
// Created by vaige on 13.3.2024.
//

#include "Terrain.h"
#include "Geometry.h"
#include "ResourceManager.h"

static constexpr int gridWidth{100};
static constexpr int gridHeight{100};

Terrain::Terrain()
:   mPerlin(gridWidth + 1, gridHeight + 1, 1),
    mTerrainScale(40, 50, 40)
{
    model = new Model();
    model->meshes.push_back(Geometry::makePerlinTerrain(
            mPerlin,
            10,
            mTerrainScale,
            {ResourceManager::instance().getTexture("dirt")})
            );
}

Terrain::~Terrain()
{
    delete model;
}

std::optional<float> Terrain::height(const glm::vec3 &pos)
{
    float x = pos.x / mTerrainScale.x;
    float z = pos.z / mTerrainScale.z;
    if (x > 0 && x < gridWidth && z > 0 && z < gridHeight) {
        return mPerlin(glm::vec2(x, z)) * mTerrainScale.y;
    }
    else {
        return {};
    }
}

void Terrain::render(Shader &shader, const glm::mat4 &parentTransform)
{
    const auto& modelMatrix = parentTransform * transform.modelMatrix();
    shader.setMat4("model", modelMatrix);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(modelMatrix)));
    model->draw(shader);
}

void Terrain::loadTerrain(glm::vec3 scale)
{
    mTerrainScale = scale;
    model->meshes.clear();
    model->meshes.push_back(Geometry::makePerlinTerrain(
            mPerlin,
            10,
            mTerrainScale,
            {ResourceManager::instance().getTexture("dirt")})
    );
}

void Terrain::onGUI()
{
    GameObject::onGUI();
    static glm::vec3 tscale;
    ImGui::DragFloat3("Terrain Scale", &tscale[0], 0.1f);
    if (ImGui::Button("Reload terrain")) {
        loadTerrain(tscale);
    }
}