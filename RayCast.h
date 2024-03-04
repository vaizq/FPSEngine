//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_RAYCAST_H
#define FPSFROMSCRATCH_RAYCAST_H


#include "BoundingVolume.h"

struct RayCast
{
    glm::vec3 startPosition;
    glm::vec3 direction;
};

struct RayHit
{
    glm::vec3 point;
    glm::vec3 offDirection;
};

bool doesHit(const RayCast& ray, const BoundingVolume& boundingVolume)
{
    return std::visit([&](auto&& shape) -> bool {
        using T = std::decay_t<decltype(shape)>;
        if constexpr (std::is_same_v<T, Sphere>) {
            const auto& sphereCenter= boundingVolume.transform.position;
            auto diff = sphereCenter - ray.startPosition;
            glm::vec3 p = ray.startPosition + glm::normalize(ray.direction) * glm::length(diff);
            return glm::length(sphereCenter - p) < shape.radius;
        }
        else if constexpr (std::is_same_v<T, Cuboid>) {
            return false;
        }
    }, boundingVolume.shape);
}


#endif //FPSFROMSCRATCH_RAYCAST_H
