//
// Created by vaige on 6.3.2024.
//

#ifndef FPSFROMSCRATCH_RANDOM_H
#define FPSFROMSCRATCH_RANDOM_H

#include <random>


float randomFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(gen);
}

#endif //FPSFROMSCRATCH_RANDOM_H
