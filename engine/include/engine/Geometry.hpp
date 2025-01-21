//
// Created by vaige on 24.2.2024.
//

#ifndef FPSFROMSCRATCH_GEOMETRY_H
#define FPSFROMSCRATCH_GEOMETRY_H

#include "Mesh.hpp"
#include <glm/glm.hpp>
#include "Perlin.hpp"

namespace Geometry
{
    Mesh makeSphere(int nVertices = 10);
    Mesh makeBox(const glm::vec3 &size = glm::vec3{1.0f, 1.0f, 1.0f});
    Mesh makeBoxWireframe(const glm::vec3 &size = glm::vec3{1.0f, 1.0f, 1.0f});
    Mesh makePlane(float width, float height, std::vector<Texture> textures = {});
    Mesh makeGrid(int width, int depth, std::vector<Texture> textures = {});
    Mesh makeCoordinateMesh();
    Mesh makeXAxis();
    Mesh makeYAxis();
    Mesh makeZAxis();
    Mesh makeCrosshire();
    Mesh makePerlinTerrain(Perlin2D& perlin, int gridSize = 10.f, glm::vec3 scale = glm::vec3{1.0f}, std::vector<Texture> textures = {});
}

#endif //FPSFROMSCRATCH_GEOMETRY_H








