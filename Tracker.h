//
// Created by vaige on 1.3.2024.
//

#ifndef FPSFROMSCRATCH_TRACKER_H
#define FPSFROMSCRATCH_TRACKER_H

#include "Entity.h"
#include <cmath>



// Rotate entity to face a point
void setDirection(Entity& e, const glm::vec3& p)
{
    auto v = glm::normalize(p - e.deltaPosition);

    e.deltaYaw = atan2(v.x, v.z);
    e.deltaPitch = -asin(v.y);
}



#endif //FPSFROMSCRATCH_TRACKER_H
