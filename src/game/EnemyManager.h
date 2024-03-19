//
// Created by vaige on 19.3.2024.
//

#ifndef FPSFROMSCRATCH_ENEMYMANAGER_H
#define FPSFROMSCRATCH_ENEMYMANAGER_H

#include "Enemy.h"
#include "Player.h"
#include <queue>


// Manages lifetimes of enemies
class EnemyManager : public GameObject
{
public:
    EnemyManager();
    void ready() override;
    void update(Duration dt);
    void shutdown() override;
private:
    Player* mPlayer{};
    std::queue<Enemy*> mQueueToHeaven;
};


#endif //FPSFROMSCRATCH_ENEMYMANAGER_H
