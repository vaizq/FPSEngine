//
// Created by vaige on 19.3.2024.
//

#include "EnemyManager.h"
#include "../engine/Random.h"


EnemyManager::EnemyManager()
{
    auto skeletonModel = &ResourceManager::instance().getModel("skeleton");

    for (int i = 0; i < 10; ++i) {
        auto enemy = std::make_unique<Enemy>("enemy" + std::to_string(i));
        enemy->parent = this;
        enemy->model = skeletonModel;
        children.push_back(std::move(enemy));
    }
}

void EnemyManager::ready()
{
    // Create some enemies
    mPlayer = findGameObject<Player>("Player");

    auto handleShot = [this](RayCast ray) {

        Enemy* shotEnemy{};
        float closest = std::numeric_limits<float>::max();

        for (auto& obj : children) {
            BoundingVolume boundingVolume{
                Transform{obj->worldTransform().modelMatrix() * obj->bounds.transform.modelMatrix()},
                obj->bounds.shape
            };

            const auto hit = intersects(ray, boundingVolume);

            if (hit) {
                if (float dist = glm::length(*hit - boundingVolume.transform.position);
                        hit != std::nullopt && dist < closest) {
                    closest = dist;
                    shotEnemy = dynamic_cast<Enemy*>(obj.get());
                }
            }
        }

        if (shotEnemy != nullptr) {
            shotEnemy->signalDead.connect([this](Enemy* enemy) {
                mQueueToHeaven.push(enemy);
            });
            shotEnemy->doDead();
        }
    };

    mPlayer->signalShoot.connect(handleShot);

    for (auto& obj : children) {
        auto enemy = dynamic_cast<Enemy*>(obj.get());
        enemy->transform.position = mPlayer->transform.position + glm::vec3(randomFloat() * 500 - 250, randomFloat() * 1000 + 100, randomFloat() * 500 - 250);
    }
}

void EnemyManager::update(Duration dt)
{
    while (!mQueueToHeaven.empty()) {
        auto enemy = mQueueToHeaven.front();
        mQueueToHeaven.pop();

        children.erase(std::remove_if(children.begin(), children.end(), [enemy](std::unique_ptr<GameObject> &obj)
        {
            return obj.get() == enemy;
        }), children.end());
    }

    // Handle collisions between enemies
    for(auto a = children.begin(); a != children.end(); ++a) {
        for(auto b = a + 1; b != children.end(); ++b) {
            auto diff = (*b)->transform.position - (*a)->transform.position;
            if (glm::length(diff) < 3.0f) {
                auto dd = 1.5f * glm::normalize(diff);
                (*b)->transform.position += dd;
                (*a)->transform.position -= dd;
            }
        }
    }
}

void EnemyManager::shutdown()
{
    children.clear();
}


