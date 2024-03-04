//
// Created by vaige on 4.3.2024.
//

#include "Entity.h"
#include <nlohmann/json.hpp>


nlohmann::json Entity::serialize(const Entity &entity)
{
    return nlohmann::json {
            {"initialTransform", Transform::serialize(entity.initialTransform)},
            {"deltaTransform", Transform::serialize(entity.deltaTransform)}
    };
}

Entity Entity::deserialize(const nlohmann::json &j)
{
    Entity entity{};
    entity.initialTransform = Transform::deserialize(j["initialTransform"]);
    entity.deltaTransform = Transform::deserialize(j["deltaTransform"]);
    return entity;
}
