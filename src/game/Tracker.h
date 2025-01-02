//
// Created by vaige on 1.3.2024.
//

#ifndef FPSFROMSCRATCH_TRACKER_H
#define FPSFROMSCRATCH_TRACKER_H

#include "engine/GameObject.h"
#include <cmath>


void alignYaw(const glm::vec3& target, Transform& transform)
{
    // First rotate on the xz plane (yaw)
    const auto xzDir = [&]() {
        auto result = target - transform.position;
        result.y = 0.0f;
        return glm::normalize(result);
    }();

    const float rotationAngle = std::atan2(xzDir.z, xzDir.x) + glm::radians(90.0f);

    if (std::abs(rotationAngle) > glm::epsilon<float>()) {
        transform.rotation = glm::angleAxis(-rotationAngle, glm::vec3{0.0f, 1.0f, 0.0f});
    }
}

void alignPitch(const glm::vec3& target, Transform& transform)
{
    const auto dir = [&]() {
        auto result = target - transform.position;
        result.x = 0.0f;
        return glm::normalize(result);
    }();

    const auto front = transform.front();
    const float rotationAngle = std::atan2(glm::length(glm::cross(dir, front)), glm::dot(dir, front));

    std::cout << glm::dot(dir, transform.right()) << std::endl;

    if (std::abs(rotationAngle) > glm::epsilon<float>()) {
        transform.rotation = glm::angleAxis(rotationAngle, transform.right()) * transform.rotation;
    }
}

// Align transform to face the target
void alignTo(const glm::vec3& target, Transform& transform)
{
    alignYaw(target, transform);
    alignPitch(target, transform);
}

void alignTo(const glm::vec3& target, GameObject& entity, const glm::mat4& parentTransform = glm::mat4{1.f})
{
    // Transform entity to world coordinates
    Transform worldTransform{parentTransform * entity.transform.modelMatrix()};
    // Align to target
    alignTo(target, worldTransform);
    // Transform back to local coordinates
    entity.transform = Transform{glm::inverse(parentTransform) * worldTransform.modelMatrix()};
}



#endif //FPSFROMSCRATCH_TRACKER_H
