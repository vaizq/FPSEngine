//
// Created by vaige on 4.3.2024.
//

#include "GameObject.h"
#include <nlohmann/json.hpp>


nlohmann::json GameObject::serialize(const GameObject &entity)
{
    return nlohmann::json {
            {"transform", Transform::serialize(entity.transform)},
            {"boundsTransform", Transform::serialize(entity.bounds.transform)}
    };
}

void GameObject::deserialize(const nlohmann::json &j, GameObject& obj)
{
    obj.transform = Transform::deserialize(j["transform"]);
    obj.bounds.transform = Transform::deserialize(j["boundsTransform"]);
}
