//
// Created by vaige on 4.3.2024.
//
#include "Transform.h"
#include <nlohmann/json.hpp>

nlohmann::json Transform::serialize(const Transform &transform)
{
    return nlohmann::json {
            {"position", nlohmann::json{{"x", transform.position.x}, {"y", transform.position.y}, {"z", transform.position.z}}},
            {"yaw", transform.yaw},
            {"pitch", transform.pitch},
            {"scale", transform.scale}
    };
}

Transform Transform::deserialize(const nlohmann::json &j)
{
    return Transform{glm::vec3{j["position"]["x"], j["position"]["y"], j["position"]["z"]},
                     j["yaw"],
                     j["pitch"],
                     j["scale"]};
}
