//
// Created by vaige on 19.3.2024.
//

#include <random>
#include "Random.h"

float randomFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(gen);
}