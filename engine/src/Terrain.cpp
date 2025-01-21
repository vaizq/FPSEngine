//
// Created by vaige on 13.3.2024.
//

#include "Terrain.hpp"
#include "Geometry.hpp"
#include "ResourceManager.hpp"

static constexpr int gridWidth{100};
static constexpr int gridHeight{100};

Terrain::Terrain()
{
    model = new Model();
    model->meshes.push_back(Geometry::makeGrid(
            100,
            100,
            {ResourceManager::instance().getTexture("dirt")})
            );
}

Terrain::~Terrain()
{
    delete model;
}
