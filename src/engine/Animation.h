//
// Created by vaige on 21.3.2024.
//

#ifndef FPSFROMSCRATCH_ANIMATION_H
#define FPSFROMSCRATCH_ANIMATION_H

#include "Transform.h"
#include "Clock.h"


struct Animation
{
    void start()
    {
        doneDuration = Duration{};
    }

    void update(Duration dt)
    {
        doneDuration += dt;
    }

    [[nodiscard]] Transform currentPose() const
    {
        float t = doneDuration.count() / duration.count();
        t = t > 1 ? 1 : t;
        return lerp(startPose, endPose, t);
    }

    [[nodiscard]] bool isDone() const
    {
        return doneDuration.count() / duration.count() >= 1.0f;
    }

    Transform startPose;
    Transform endPose;
    Duration duration;

private:
    Duration doneDuration;
};

#endif //FPSFROMSCRATCH_ANIMATION_H
