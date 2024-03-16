//
// Created by vaige on 4.3.2024.
//
#include "Transform.h"
#include <nlohmann/json.hpp>
#include "Util.h"

nlohmann::json Transform::serialize(const Transform &transform)
{
    return nlohmann::json {
            {"position", Util::serialize(transform.position)},
            {"rotation", Util::serialize(transform.rotation)},
            {"scale", transform.scale}};
}

Transform Transform::deserialize(const nlohmann::json &j)
{
    return Transform {
        Util::glm::vec3::deserialize(j["position"]),
        Util::glm::vec3::deserialize(j["rotation"]),
        j["scale"]};
}
