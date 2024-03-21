//
// Created by vaige on 5.3.2024.
//

#ifndef FPSFROMSCRATCH_WEAPON_H
#define FPSFROMSCRATCH_WEAPON_H


#include "../engine/GameObject.h"
#include "../engine/AudioSource.h"
#include "../boost/signals2.hpp"

class Weapon : public GameObject
{
public:
    Weapon()
    {
        AudioManager::instance(); // make sure audio manager is initialized
        mAudioSource = std::make_unique<AudioSource>();
    }

    void pressTrigger()
    {
        mTriggerPressed = true;
        fire();
    }

    void releaseTrigger()
    {
        mTriggerPressed = false;
    }

    void update(Duration dt) override
    {
        mAudioSource->setTransform(worldTransform());

        if (mTriggerPressed && (mFromLastFire += dt) > mFiringInterval) {
            fire();
        }
    }

    boost::signals2::signal<void()> signalFire;

private:
    void fire()
    {
        signalFire();
        mAudioSource->playAudio("ak47Fire", false);
        mFromLastFire = Duration{0.0f};
    }

    std::unique_ptr<AudioSource> mAudioSource;
    bool mTriggerPressed{false};
    float mFireRate{400}; // rounds / min
    Duration mFiringInterval{1.0f / mFireRate * 60.0f};
    Duration mFromLastFire{};
};

#endif //FPSFROMSCRATCH_WEAPON_H
