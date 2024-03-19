//
// Created by vaige on 19.3.2024.
//

#include "Enemy.h"
#include "../engine/ResourceManager.h"


Enemy::Enemy(const std::string& name)
: GameObject{}
{
    this->name = name;
    model = &ResourceManager::instance().getModel("skeleton");
    bounds.transform.position.y = 3.0f;
    auto& shape = std::get<Sphere>(bounds.shape);
    shape.radius = 1.0f;
}


void Enemy::ready()
{
    mPlayer = findGameObject<Player>("Player");
    mTerrain = findGameObject<Terrain>("terrain");
}


void Enemy::update(Duration dt)
{
    auto diff = mPlayer->transform.position - transform.position;
    diff.y = 0.0f;
    if (glm::length(diff) > 3.0f) {
        transform.position += mSpeed * glm::normalize(diff) * dt.count();
    }

    auto groundHeight = mTerrain->height(transform.position);

    if (groundHeight) {
        transform.position += gravity * dt.count();

        if (transform.position.y < *groundHeight + mHeight) {
            transform.position.y = *groundHeight + mHeight;
        }
    }
}