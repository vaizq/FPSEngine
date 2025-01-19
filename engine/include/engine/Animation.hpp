//
// Created by vaige on 21.3.2024.
//

#ifndef FPSFROMSCRATCH_ANIMATION_H
#define FPSFROMSCRATCH_ANIMATION_H

#include "Clock.hpp"
#include <map>
#include <glm/gtc/quaternion.hpp>
#include "Skeleton.hpp"


template<typename T>
struct KeyFrame {
    double time;
    T key;
};

struct AnimationChannel {
    std::vector<KeyFrame<glm::vec3>> positionKeys;
    std::vector<KeyFrame<glm::quat>> rotationKeys;
    std::vector<KeyFrame<glm::vec3>> scalingKeys;

    glm::mat4 getTransform(double t) const;
};

struct Animation {
    using Duration = std::chrono::duration<double>;
    using Time = Clock::time_point;

    std::string name;
    double duration;
    std::map<std::string, AnimationChannel> channels;

    double update(double time, double dt) const;
    void update(double time, Skeleton& skeleton) const;
};

#endif //FPSFROMSCRATCH_ANIMATION_H
