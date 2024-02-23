//
// Created by vaige on 19.2.2024.
//

#include "FPSGame.h"
#include <iostream>
#include "Util.h"


static std::vector<Vertex> squareMesh()
{
    return {
        Vertex{glm::vec3{0.5f, 0.5f, 0.0f}, glm::vec3{}, glm::vec2{}},
        Vertex{glm::vec3{0.5f, -0.5f, 0.0f}, glm::vec3{}, glm::vec2{}},
        Vertex{glm::vec3{-0.5f, -0.5f, 0.0f}, glm::vec3{}, glm::vec2{}},
        Vertex{glm::vec3{-0.5f, 0.5f, 0.0f}, glm::vec3{}, glm::vec2{}}
    };
}

static std::vector<Vertex> vertices = squareMesh();

static std::vector<unsigned int> indices = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
};

FPSGame::FPSGame()
:   mShader{Util::getShaderPath("vertex.glsl").c_str(), Util::getShaderPath("frag.glsl").c_str()}
{
    mMesh = std::make_unique<Mesh>(vertices, indices, std::vector<Texture>{});
}


void FPSGame::update(Game::Duration dt)
{
    mPos += dt.count() * mVelocity;
}

void FPSGame::render()
{
    mShader.use();
    mShader.setVec3("dPos", mPos);
    mMesh->draw(mShader);
}

void FPSGame::handleInput()
{
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS) {
        mVelocity.y = 1.0f;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS) {
        mVelocity.y = -1.0f;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS) {
        mVelocity.x = -1.0f;
    }
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS) {
        mVelocity.x = 1.0f;
    }
}



















