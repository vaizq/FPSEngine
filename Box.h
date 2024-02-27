//
// Created by vaige on 26.2.2024.
//

#ifndef FPSFROMSCRATCH_BOX_H
#define FPSFROMSCRATCH_BOX_H

#include "Mesh.h"

struct Box
{
    Mesh mesh;
    glm::vec3 pos;
    glm::vec3 size;
};

#endif //FPSFROMSCRATCH_BOX_H
