//
// Created by vaige on 4.3.2024.
//
#include "RayCast.hpp"

std::optional<glm::vec3> intersects(const RayCast& ray, const BoundingVolume& boundingVolume)
{
    return std::visit([&](auto&& shape) -> std::optional<glm::vec3> {
        using T = std::decay_t<decltype(shape)>;
        if constexpr (std::is_same_v<T, Sphere>) {
            const auto& sphereCenter= boundingVolume.transform.position;
            auto diff = sphereCenter - ray.startPosition;
            const auto scale = boundingVolume.transform.scale;
            const float r = ((scale.x + scale.y + scale.z) / 3.0f) * shape.radius;
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
