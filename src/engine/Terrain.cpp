//
// Created by vaige on 13.3.2024.
//

#include "Terrain.h"
#include "Geometry.h"
#include "ResourceManager.h"

Terrain::Terrain()
{
    model = new Model();
    model->meshes.push_back(Geometry::makePerlinTerrain(
            100,
            100,
            10,
            glm::vec3{1.0f},
            {ResourceManager::instance().getTexture("dirt")})
            );
}

Terrain::~Terrain()
{
    delete model;
}

float Terrain::height(const glm::vec3 &pos)
{
    return 0;
}

void Terrain::render(Shader &shader, const glm::mat4 &parentTransform)
{
    const auto& modelMatrix = parentTransform * transform.modelMatrix();
    shader.setMat4("model", modelMatrix);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(modelMatrix)));
    model->draw(shader);
}