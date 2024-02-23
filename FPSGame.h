//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_FPSGAME_H
#define FPSFROMSCRATCH_FPSGAME_H


#include "Game.h"
#include <glm/vec3.hpp>
#include "Shader.h"
#include "Mesh.h"
#include <memory>


class FPSGame : public Game
{
public:
    FPSGame();

    void handleInput() override;

    void update(Duration dt) override;

    void render() override;

private:
    Shader mShader;
    std::unique_ptr<Mesh> mMesh;
    glm::vec3 mPos{};
    glm::vec3 mVelocity{};
};


#endif //FPSFROMSCRATCH_FPSGAME_H
