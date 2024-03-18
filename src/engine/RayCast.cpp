//
// Created by vaige on 4.3.2024.
//
#include "RayCast.h"

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

// TODO modify terrain so that Terrain::height() is const function
std::optional<glm::vec3> intersects(const RayCast& ray, Terrain& terrain)
{
    // Assume that the ray starts above the terrain
    if (ray.startPosition.x < 0.0f ||
        ray.startPosition.z < 0.0f ||
        ray.startPosition.x > terrain.width() ||
        ray.startPosition.z > terrain.depth() ||
        ray.startPosition.y < terrain.height(ray.startPosition))
    {
        return {};
    }

    const float delta = 0.1f;

    for (float x{delta}; ; x += delta) {
        auto pos = ray.startPosition + x * ray.direction;
        auto height = terrain.height(pos);
        if (height == std::nullopt) {
            return {};
        }
        if (pos.y < terrain.height(pos)) {
            return pos;
        }
    }
}