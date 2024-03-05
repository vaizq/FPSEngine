//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_PLAYER_H
#define FPSFROMSCRATCH_PLAYER_H


#include "../core/Model.h"
#include "../core/Shader.h"
#include "../core/Camera.h"
#include "../core/Util.h"
#include "../core/GameObject.h"
#include <imgui.h>
#include "../core/InputManager.h"

class Player : public GameObject
{
public:
    Player(Camera& camera, Model* gunModel)
    : mCamera{camera}
    {
        name = "Player";
        auto gun = std::make_unique<GameObject>();
        gun->name = "PlayersWeapon";
        gun->model = gunModel;
        children.push_back(std::move(gun));

        mPrevMousePos = InputManager::instance().mousePos();
    }

    void update(std::chrono::duration<float> dt) override
    {
        updatePosition(dt);
        updateRotation(dt);
        mCamera.getTransform() = transform;
    }

private:
    void updatePosition(std::chrono::duration<float> dt)
    {
        if (InputManager::instance().keyPressed(GLFW_KEY_W)) {
            mVelocity.z = -1.f;
        }
        else if (InputManager::instance().keyPressed(GLFW_KEY_S)) {
            mVelocity.z = 1.f;
        }
        else {
            mVelocity.z = 0.0f;
        }

        if (InputManager::instance().keyPressed(GLFW_KEY_A)) {
            mVelocity.x = -1.f;
        }
        else if (InputManager::instance().keyPressed(GLFW_KEY_D)) {
            mVelocity.x = 1.f;
        }
        else {
            mVelocity.x = 0.0f;
        }

        mVelocity.y = 0.0f;

        if (glm::length(mVelocity) > std::numeric_limits<float>::epsilon()) {
            transform.position += mSpeed * glm::normalize(mVelocity) * dt.count();
        }
    }

    void updateRotation(std::chrono::duration<float> dt)
    {
        auto mousePos = InputManager::instance().mousePos();
        float dx = mousePos.x - mPrevMousePos.x;
        float dy = mousePos.y - mPrevMousePos.y;

        transform.yaw += mSensitivity * dx;
        transform.pitch += mSensitivity * dy;

        mPrevMousePos = mousePos;
    }

    Camera& mCamera;
    glm::vec3 mVelocity{};
    float mSpeed{1.0f};
    glm::vec2 mPrevMousePos{};
    float mSensitivity{0.001f};
};

#endif //FPSFROMSCRATCH_PLAYER_H
