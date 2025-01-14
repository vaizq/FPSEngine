#include "Animation.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

double Animation::update(double time, double dt) {
    return std::fmod(time + dt, duration);
}

void Animation::update(double time, Skeleton& skeleton) {
    Joint& root = skeleton[0];

    if (auto it = channels.find(root.name); it != channels.end()) {
        root.finalTransform = it->second.getTransform(time);
    } else {
        root.finalTransform = root.transformation;
    }

    for (int i = 1; i < skeleton.size(); i++) {
        Joint& joint = skeleton[i];
        if (auto it = channels.find(joint.name); it != channels.end()) {
            joint.finalTransform = skeleton[joint.parent].finalTransform * it->second.getTransform(time);
        } else {
            joint.finalTransform = skeleton[joint.parent].finalTransform * joint.transformation;
        }
    }
}

glm::vec3 interpolate(const KeyFrame<glm::vec3>& a, const KeyFrame<glm::vec3>& b, double time) {
    const float t = (time - a.time) / (b.time - a.time);
    return (1 - t) * a.key + t * b.key;
}

glm::quat interpolate(const KeyFrame<glm::quat>& a, const KeyFrame<glm::quat>& b, double time) {
    const float t = (time - a.time) / (b.time - a.time);
    return glm::slerp(a.key, b.key, t);
}

template <typename T>
T linearInterpolate(const std::vector<KeyFrame<T>>& frames, double time) {
    if (frames.empty()) {
        throw std::runtime_error("Frames is empty");
    }
    auto frameIter = frames.begin();
    for(; frameIter != frames.end() && time > frameIter->time; ++frameIter) {}

    if (frameIter == frames.begin()) {
        return interpolate(frames.back(), *frameIter, time);
    } else if (frameIter == frames.end()) { 
        return frames.back().key;
    }

    return interpolate(*(frameIter-1), *frameIter, time);
}

glm::mat4 AnimationChannel::getTransform(double time) const {
    glm::vec3 pos = linearInterpolate(positionKeys, time);
    glm::quat rotation = linearInterpolate(rotationKeys, time);
    glm::vec3 scale = linearInterpolate(scalingKeys, time);

    return glm::translate(glm::mat4{1.0f}, pos) * glm::mat4(rotation) * glm::scale(glm::mat4{1.0f}, scale);
}
