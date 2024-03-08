//
// Created by vaige on 5.3.2024.
//

#ifndef FPSFROMSCRATCH_WEAPON_H
#define FPSFROMSCRATCH_WEAPON_H


#include "../engine/GameObject.h"
#include "../engine/AudioSource.h"


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
        mAudioSource->playAudio("ak47Fire", true);
    }

    void releaseTrigger()
    {
        mAudioSource->stopAudio();
    }

    void update(Duration dt) override
    {
        mAudioSource->setTransform(worldTransform());
    }

private:
    std::unique_ptr<AudioSource> mAudioSource;
};

#endif //FPSFROMSCRATCH_WEAPON_H
