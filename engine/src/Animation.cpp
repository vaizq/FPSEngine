#include "Animation.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

void Animation::update(double dt) {
    time = std::fmod(time + dt, duration);
}

void Animation::update(Skeleton& skeleton) {
    Joint& root = skeleton[0];
    root.finalTransform = channels[root.name].getTransform(time);

    for (int i = 1; i < skeleton.size(); i++) {
        Joint& joint = skeleton[i];
        if (auto it = channels.find(joint.name); it != channels.end()) {
            const AnimationChannel& ch = it->second;
            joint.finalTransform = skeleton[joint.parent].finalTransform * ch.getTransform(time);
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
    /*
    size_t idx = (size_t) time;

    glm::vec3 pos = positionKeys[idx].key;//linearInterpolate(positionKeys, time);
    glm::quat rotation = rotationKeys[idx].key;//linearInterpolate(rotationKeys, time);
    glm::vec3 scale = scalingKeys[idx].key;//linearInterpolate(scalingKeys, time);

    return glm::translate(glm::mat4{1.0f}, pos) * glm::mat4(rotation) * glm::scale(glm::mat4{1.0f}, scale);
    */
}
