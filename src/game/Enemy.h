//
// Created by vaige on 19.3.2024.
//

#ifndef FPSFROMSCRATCH_ENEMY_H
#define FPSFROMSCRATCH_ENEMY_H

#include "../engine/GameObject.h"
#include "Player.h"

class Enemy : public GameObject
{
public:
    Enemy(const std::string& name);
    void ready() override;
    void update(Duration dt) override;
private:
    Player* mPlayer;
    Terrain* mTerrain;
    float mSpeed{10.0f};
    static constexpr float mHeight{5.0f};
};


#endif //FPSFROMSCRATCH_ENEMY_H
