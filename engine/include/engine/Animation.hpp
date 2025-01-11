//
// Created by vaige on 21.3.2024.
//

#ifndef FPSFROMSCRATCH_ANIMATION_H
#define FPSFROMSCRATCH_ANIMATION_H

#include "Transform.hpp"
#include "Clock.hpp"


struct Animation
{
    void start(float loop = false)
    {
        doneDuration = Duration{};
        mRunning = true;
    }

    void update(Duration dt)
    {
        doneDuration += dt;
    }

    [[nodiscard]] Transform currentPose()
    {
        if (duration == Duration{}) {
            return startPose;
        }
        float t = doneDuration.count() / duration.count();

        if (t >= 1) {
            t = 1;
            mRunning = false;
        }

        return lerp(startPose, endPose, t);
    }

    [[nodiscard]] bool isRunning() const
    {
        return mRunning;
    }

    [[nodiscard]] bool isDone() const
    {
        return doneDuration.count() / duration.count() >= 1.0f;
    }

    Transform startPose{};
    Transform endPose{};
    Duration duration{};

private:
    bool mRunning{false};
    Duration doneDuration{};
};


struct AnimationSequence
{
    AnimationSequence(const std::vector<Transform>& poses, const std::vector<Duration>& durations)
    {
        assert(poses.size() == durations.size());

        auto pose = poses.begin();
        auto duration = durations.begin();

        while (pose != poses.end()) {
            Animation anim;
            anim.startPose = *pose++;
            anim.endPose = *pose;
            anim.duration = *duration++;
            mAnimations.push_back(anim);
        }
        Animation anim;
        anim.startPose = poses.back();
        anim.endPose = poses.front();
        anim.duration = *duration;
        mAnimations.push_back(anim);
    }

    void start()
    {
        mCurrent = mAnimations.begin();
        mCurrent->start();
    }

    void update(Duration dt) {
        if (mCurrent->isDone()) {
            ++mCurrent;
            if (mCurrent == mAnimations.end()) {
                mCurrent = mAnimations.begin();
            }
            mCurrent->start();
        }

        mCurrent->update(dt);
    }

    Transform currentPose() {
        return mCurrent->currentPose();
    }

private:
    std::vector<Animation> mAnimations;
    std::vector<Animation>::iterator mCurrent;
};









#endif //FPSFROMSCRATCH_ANIMATION_H
