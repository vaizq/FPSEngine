//
// Created by vaige on 19.3.2024.
//

#include "Enemy.h"
#include "../engine/ResourceManager.h"
#include <boost/signals2.hpp>


Enemy::Enemy(const std::string& name)
: GameObject{}
{
    this->name = name;
    model = &ResourceManager::instance().getModel("skeleton");
    bounds.transform.position.y = 3.0f;
    auto& shape = std::get<Sphere>(bounds.shape);
    shape.radius = 0.7f;
}


void Enemy::ready()
{
    mPlayer = findGameObject<Player>("Player");
    mTerrain = findGameObject<Terrain>("terrain");

    mBreathAudio.playAudio("enemy-breathing", true);
}


void Enemy::update(Duration dt)
{
    if (mDoDeadAnimation && !mDeadAnimation.isDone()) {
        updateDeadAnimation(dt);
    }
    else {
        updateMovement(dt);
    }

    mBreathAudio.setTransform(worldTransform());
    mScreamAudio.setTransform(worldTransform());
}

void Enemy::doDead()
{
    mDoDeadAnimation = true;
    mDeadAnimation.startPose = transform;
    mDeadAnimation.endPose = [this]() {
        Transform result{transform};
        result.position += glm::vec3{0.0f, -8.0f, 0.0f};
        return result;
    }();
    mDeadAnimation.duration = 1s;

    mScreamAudio.playAudio("enemy-scream");
}

void Enemy::updateMovement(Duration dt)
{
    const auto diff = [this]()
    {
        auto result = mPlayer->transform.position - transform.position;
        result.y = 0;
        return result;
    }();

    const auto diffDir = glm::normalize(diff);
    const auto front = [this]()
    {
        auto result = transform.front();
        result.y = 0;
        return glm::normalize(result);
    }();

    // Rotate and then move forward and apply gravity

    const auto rotationAxis = glm::cross(front, diffDir);
    const float rotationAngle = glm::asin(glm::length(rotationAxis));

    if (rotationAngle > glm::epsilon<float>()) {
        transform.rotation = glm::rotate(transform.rotation, rotationAngle, glm::normalize(rotationAxis));
    }

    if (glm::length(diff) > 3.0f) {
        transform.position += mSpeed * glm::normalize(transform.front()) * dt.count();
    }

    auto groundHeight = mTerrain->height(transform.position);

    if (groundHeight) {
        transform.position += gravity * dt.count();

        if (transform.position.y < *groundHeight + mHeight) {
            transform.position.y = *groundHeight + mHeight;
        }
    }
}

void Enemy::updateDeadAnimation(Duration dt)
{
    mDeadAnimation.update(dt);
    transform = mDeadAnimation.currentPose();
    if (mDeadAnimation.isDone()) {
        signalDead(this);
    }
}