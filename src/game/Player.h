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
#include "../engine/Terrain.h"
#include "../engine/RayCast.h"
#include "../engine/Renderer.h"

static glm::vec3 gravity{0.0f, -90.0f, 0.0f};

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
            auto t = worldTransform();
            RayCast ray(t.position, t.front());
            if (auto iPoint = intersects(ray, *mTerrain)) {
                auto bulletHit = std::make_unique<GameObject>();
                static int nameIndex{0};
                bulletHit->name = "bullethit" + std::to_string(nameIndex++);
                bulletHit->transform.position = *iPoint;
                bulletHit->parent = mTerrain;
                bulletHit->model = &ResourceManager::instance().getModel("eyeBall");
                getScene()->children.push_back(std::move(bulletHit));
            }
        };
        InputManager::instance().buttonReleaseHandlers[GLFW_MOUSE_BUTTON_LEFT] = [this] ()
        {
            mWeapon->releaseTrigger();
        };

        InputManager::instance().keyPressHandlers[GLFW_KEY_SPACE] = [this] ()
        {
            if (transform.position.y <= mGroundLevel + mPlayerHeight) {
                mVelocity.y = 40.0f;
                transform.position.y += 0.1f;
            }
        };
        InputManager::instance().keyPressHandlers[GLFW_KEY_LEFT_SHIFT] = [this] ()
        {
            transform.position.y -= 3.0f;
            mPlayerHeight = 3.0f;
        };
        InputManager::instance().keyReleaseHandlers[GLFW_KEY_LEFT_SHIFT] = [this] ()
        {
            transform.position.y += 3.0f;
            mPlayerHeight = 6.0f;
        };
    }

    ~Player() override
    {
        mWeapon->transform = mWeaponRegularTransform;
    }

    void ready() override
    {
        mTerrain = findGameObject<Terrain>("terrain");
    }

    void update(std::chrono::duration<float> dt) override
    {
        mGroundLevel = [this]() {
            auto result = mTerrain->height(transform.position);
            if (result) {
                return *result;
            }
            else {
                return std::numeric_limits<float>::min();
            }
        }();

        if (enableInput) {
            updatePosition(dt);
        }
        updateRotation(dt);
        mCamera.getTransform() = transform;

        Renderer::instance().activeCamera = mCamera;
    }

    void onGUI() override
    {
        GameObject::onGUI();

        ImGui::DragFloat3("Gravity", &gravity[0], 0.1f);
        ImGui::DragFloat("Sensitivity", &mSensitivity, 0.0001f);
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

        if (transform.position.y > mGroundLevel + mPlayerHeight) {
            mVelocity += gravity * dt.count();
            transform.position.y += mVelocity.y * dt.count();
        }
        else {
            mVelocity.y = 0.0f;
            transform.position.y = mGroundLevel + mPlayerHeight;
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
        float dy = mousePos.y - mPrevMousePos.y;

        if (enableInput) {
            auto yaw = glm::angleAxis(-mSensitivity * dx, glm::vec3(0.0f, 1.0f, 0.0f));
            transform.rotation = yaw * transform.rotation;
            auto pitch = glm::angleAxis(-mSensitivity * dy, transform.right());
            transform.rotation = pitch * transform.rotation;
        }

        mPrevMousePos = mousePos;
    }

    Camera& mCamera;
    glm::vec3 mVelocity{};
    float mSpeed{30.0f};
    glm::vec2 mPrevMousePos{};
    float mSensitivity{0.001f};
    const Transform mWeaponAimTransform{glm::vec3{0.0f, -0.2f, -0.6f}, glm::vec3{glm::radians(1.0f), 0.0f, 0.0f}, glm::vec3{3.6f}};
    Transform mWeaponRegularTransform{};
    Weapon* mWeapon;
    float mGroundLevel{4.0f};
    Terrain* mTerrain;
    float mPlayerHeight{6};
};

#endif //FPSFROMSCRATCH_PLAYER_H