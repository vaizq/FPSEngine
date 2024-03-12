//
// Created by vaige on 12.3.2024.
//

#ifndef FPSFROMSCRATCH_LIGHT_H
#define FPSFROMSCRATCH_LIGHT_H

#include "GameObject.h"

class Light : public GameObject
{
public:
    explicit Light();

    void render(Shader& shader, const glm::mat4& parentTransform) override;

private:
    Mesh mMesh;
};


#endif //FPSFROMSCRATCH_LIGHT_H
