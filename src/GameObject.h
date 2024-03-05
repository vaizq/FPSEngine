//
// Created by vaige on 5.3.2024.
//

#ifndef FPSFROMSCRATCH_GAMEOBJECT_H
#define FPSFROMSCRATCH_GAMEOBJECT_H

#include <vector>
#include <memory>

struct GameObject
{

    std::vector<std::unique_ptr<GameObject>> children;
};

#endif //FPSFROMSCRATCH_GAMEOBJECT_H
