//
// Created by vaige on 19.3.2024.
//

#ifndef FPSFROMSCRATCH_ENEMY_H
#define FPSFROMSCRATCH_ENEMY_H

#include "../engine/GameObject.h"
#include "../engine/Animation.h"
#include "../engine/AudioSource.h"
#include "Player.h"
#include <boost/signals2.hpp>

class Enemy : public GameObject
{
public:
    Enemy(const std::string& name);
    void ready() override;
    void update(Duration dt) override;
    void doDead();
    bool isDying() const;

    boost::signals2::signal<void(Enemy*)> signalDead;
private:
    void updateMovement(Duration dt);
    void updateDeadAnimation(Duration dt);
    Player* mPlayer{};
    Terrain* mTerrain{};
    float mSpeed{10.0f};
    static constexpr float mHeight{5.0f};
    bool mDying{false};
    Animation mDeadAnimation{};
    AudioSource mBreathAudio{};
    AudioSource mScreamAudio{};
    AnimationSequence mWalkAnimation;
};


#endif //FPSFROMSCRATCH_ENEMY_H