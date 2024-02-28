//
// Created by vaige on 28.2.2024.
//

#ifndef FPSFROMSCRATCH_SKULL_H
#define FPSFROMSCRATCH_SKULL_H

#include "Entity.h"

struct SkullWithEyes
{
    SkullWithEyes(Model* skullModel, Model* eyeModel) {
        skull.model = skullModel;
        leftEye.model = eyeModel;
        rightEye.model = eyeModel;

        // Configure Entities in a right initial positions
    }

    void update() {

    }

    float yaw{};
    float pitch{};
    glm::vec3 pos{};
    Entity skull;
    Entity leftEye;
    Entity rightEye;
};

#endif //FPSFROMSCRATCH_SKULL_H
