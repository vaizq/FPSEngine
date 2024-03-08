//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_PLAYER_H
#define FPSFROMSCRATCH_PLAYER_H


#include "../engine/Model.h"
#include "../engine/Shader.h"
#include "../engine/Camera.h"
#include "../engine/Util.h"
#include "../engine/GameObject.h"
#include <imgui.h>
#include "../engine/InputManager.h"
#include "../engine/AudioSource.h"
#include "Weapon.h"
#include "../engine/ResourceManager.h"

static glm::vec3 gravity{0.0f, -10.0f, 0.0f};

class Player : public GameObject
{
public:
    Player(Camera& camera)
    : mCamera{camera}
    {
        name = "Player";
        auto gun = std::make_unique<Weapon>();
        mWeapon = gun.get();
        gun->parent = this;
        gun->name = "PlayersWeapon";
        gun->model = &ResourceManager::instance().getModel("ak47");
        children.push_back(std::move(gun));

        mPrevMousePos = InputManager::instance().mousePos();

        // Aiming "animation"
        InputManager::instance().buttonPressHandlers[GLFW_MOUSE_BUTTON_RIGHT] = [this] ()
        {
            mWeaponRegularTransform = mWeapon->transform;
            mWeapon->transform = mWeaponAimTransform;
        };
        InputManager::instance().buttonReleaseHandlers[GLFW_MOUSE_BUTTON_RIGHT] = [this] ()
        {
            mWeapon->transform = mWeaponRegularTransform;
        };

        // Shooting audio
        InputManager::instance().buttonPressHandlers[GLFW_MOUSE_BUTTON_LEFT] = [this] ()
        {
            mWeapon->pressTrigger();
        };
        InputManager::instance().buttonReleaseHandlers[GLFW_MOUSE_BUTTON_LEFT] = [this] ()
        {
            mWeapon->releaseTrigger();
        };

        InputManager::instance().keyPressHandlers[GLFW_KEY_SPACE] = [this] ()
        {
            if (transform.position.y == mGroundLevel) {
                mVelocity.y = 20.0f;
                transform.position.y += 0.1f;
            }
        };
    }

    ~Player() override
    {
        mWeapon->transform = mWeaponRegularTransform;
    }

    void update(std::chrono::duration<float> dt) override
    {
        if (enableInput) {
            updatePosition(dt);
            updateRotation(dt);
        }
        mCamera.getTransform() = transform;
    }

    void onGUI() override
    {
        GameObject::onGUI();

        ImGui::DragFloat3("Gravity", &gravity[0], 0.1f);
    }

    bool enableInput{true};

private:
    void updatePosition(std::chrono::duration<float> dt)
    {
        mVelocity.x = 0.f;
        mVelocity.z = 0.f;

        if (InputManager::instance().keyPressed(GLFW_KEY_W)) {
            auto front = transform.front();
            mVelocity.x += front.x;
            mVelocity.z += front.z;
        }
        else if (InputManager::instance().keyPressed(GLFW_KEY_S)) {
            auto front = transform.front();
            mVelocity.x -= front.x;
            mVelocity.z -= front.z;
        }

        if (InputManager::instance().keyPressed(GLFW_KEY_A)) {
            auto right = transform.right();
            mVelocity.x -= right.x;
            mVelocity.z -= right.z;
        }
        else if (InputManager::instance().keyPressed(GLFW_KEY_D)) {
            auto right = transform.right();
            mVelocity.x += right.x;
            mVelocity.z += right.z;
        }

        if (transform.position.y > mGroundLevel) {
            mVelocity += gravity * dt.count();
            transform.position.y += mVelocity.y * dt.count();
        }
        else {
            mVelocity.y = 0.0f;
            transform.position.y = mGroundLevel;
        }


        auto xzVelocity = glm::vec3(mVelocity.x, 0.0f, mVelocity.z);


        if (glm::length(xzVelocity) > std::numeric_limits<float>::epsilon()) {
            transform.position += mSpeed * glm::normalize(xzVelocity) * dt.count();
        }


        AudioManager::instance().setTransform(transform);
    }

    void updateRotation(std::chrono::duration<float> dt)
    {
        auto mousePos = InputManager::instance().mousePos();
        float dx = mousePos.x - mPrevMousePos.x;
        float dy = -(mousePos.y - mPrevMousePos.y);

        transform.yaw += mSensitivity * dx;
        transform.pitch += mSensitivity * dy;

        mPrevMousePos = mousePos;
    }

    Camera& mCamera;
    glm::vec3 mVelocity{};
    float mSpeed{30.0f};
    glm::vec2 mPrevMousePos{};
    float mSensitivity{0.001f};
    const Transform mWeaponAimTransform{glm::vec3{0.0f, -0.2f, -0.6f}, 0.0f, glm::radians(1.0f), 3.6f};
    Transform mWeaponRegularTransform{};
    Weapon* mWeapon;
    float mGroundLevel{4.0f};
};

#endif //FPSFROMSCRATCH_PLAYER_H