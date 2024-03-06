//
// Created by vaige on 6.3.2024.
//

#ifndef FPSFROMSCRATCH_PARTICLEEMITTER_H
#define FPSFROMSCRATCH_PARTICLEEMITTER_H


#include "../core/GameObject.h"


class ParticleEmitter : public GameObject
{
public:
    ParticleEmitter()
    {}

    virtual void update(Duration dt) override
    {

    }

    float rate{1.f};
    float radius{1.f};
    glm::vec3 direction{worldUp};
    float angle{glm::radians(45.f)};
private:
};

#endif //FPSFROMSCRATCH_PARTICLEEMITTER_H
