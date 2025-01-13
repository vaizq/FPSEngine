//
// Created by vaige on 21.3.2024.
//

#ifndef FPSFROMSCRATCH_ANIMATION_H
#define FPSFROMSCRATCH_ANIMATION_H

#include "Clock.hpp"
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

struct Joint {
    std::string name; // node name?
    unsigned parent;
    glm::mat4 transformation;
    glm::mat4 finalTransform;
};

using Skeleton = std::vector<Joint>;

struct BoneInfo {
    std::string name;
    glm::mat4 offsetMatrix;
};

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

    void update(double dt);

    void update(Skeleton& skeleton);

    double duration;
    double time{0.0};
    std::map<std::string, AnimationChannel> channels;
};

#endif //FPSFROMSCRATCH_ANIMATION_H
