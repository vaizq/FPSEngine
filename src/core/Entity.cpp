//
// Created by vaige on 4.3.2024.
//

#include "Entity.h"
#include <nlohmann/json.hpp>


nlohmann::json Entity::serialize(const Entity &entity)
{
    return nlohmann::json {
            {"transform", Transform::serialize(entity.transform)},
    };
}

Entity Entity::deserialize(const nlohmann::json &j)
{
    Entity entity{};
    entity.transform = Transform::deserialize(j["transform"]);
    return entity;
}
