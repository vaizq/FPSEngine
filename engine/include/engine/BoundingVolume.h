//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_BOUNDINGVOLUME_H
#define FPSFROMSCRATCH_BOUNDINGVOLUME_H

#include <glm/glm.hpp>
#include <variant>
#include "Transform.h"
#include <nlohmann/json_fwd.hpp>

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
    Transform transform;
    BoundingShape shape;


    static nlohmann::json serialize(const BoundingVolume& bounds);
    static BoundingVolume deserialize(const nlohmann::json& j);
};




#endif //FPSFROMSCRATCH_BOUNDINGVOLUME_H
