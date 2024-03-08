//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_RAYCAST_H
#define FPSFROMSCRATCH_RAYCAST_H


#include "BoundingVolume.h"
#include <iostream>

struct RayCast
{
    glm::vec3 startPosition;
    glm::vec3 direction;
};

std::optional<glm::vec3> intersects(const RayCast& ray, const BoundingVolume& boundingVolume)
{
    return std::visit([&](auto&& shape) -> std::optional<glm::vec3> {
        using T = std::decay_t<decltype(shape)>;
        if constexpr (std::is_same_v<T, Sphere>) {
            const auto& sphereCenter= boundingVolume.transform.position;
            auto diff = sphereCenter - ray.startPosition;
            const float r = boundingVolume.transform.scale * shape.radius;
            glm::vec3 p = ray.startPosition + glm::normalize(ray.direction) * glm::length(diff);
            if (glm::length(sphereCenter - p) < r) {
                return sphereCenter - glm::normalize(ray.direction) * r;
            }
            else {
                return std::nullopt;
            }
        }
        else if constexpr (std::is_same_v<T, Cuboid>) {
            return std::nullopt;
        }
    }, boundingVolume.shape);
}


#endif //FPSFROMSCRATCH_RAYCAST_H
