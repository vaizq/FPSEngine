//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_BOUNDINGVOLUME_H
#define FPSFROMSCRATCH_BOUNDINGVOLUME_H

#include <glm/glm.hpp>
#include <variant>


struct Sphere
{
    float radius;
};

struct Cuboid
{
    glm::vec3 size;
};


using BoundingShape = std::variant<Sphere, Cuboid>;


struct BoundingVolume
{
    glm::vec3 position;
    float yaw{};
    float pitch{};
};

#endif //FPSFROMSCRATCH_BOUNDINGVOLUME_H
