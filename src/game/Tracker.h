//
// Created by vaige on 1.3.2024.
//

#ifndef FPSFROMSCRATCH_TRACKER_H
#define FPSFROMSCRATCH_TRACKER_H

#include "../core/GameObject.h"
#include <cmath>



// Align transform to face the target
void alignTo(const glm::vec3& target, Transform& transform)
{
    auto v = glm::normalize(target - transform.position);
    transform.yaw = atan2(v.x, v.z) + glm::radians(180.0f);
    transform.pitch = asin(v.y);
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
