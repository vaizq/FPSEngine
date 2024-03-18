//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_RAYCAST_H
#define FPSFROMSCRATCH_RAYCAST_H


#include "BoundingVolume.h"
#include <iostream>
#include "Terrain.h"

struct RayCast
{
    glm::vec3 startPosition;
    glm::vec3 direction;
};

std::optional<glm::vec3> intersects(const RayCast& ray, const BoundingVolume& boundingVolume);
std::optional<glm::vec3> intersects(const RayCast& ray, Terrain& terrain);




#endif //FPSFROMSCRATCH_RAYCAST_H