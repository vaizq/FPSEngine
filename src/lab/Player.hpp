#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "engine/GameObject.hpp"
#include "engine/Camera.hpp"
#include "engine/InputManager.hpp"
#include "engine/ResourceManager.hpp"


class Player : public GameObject {
public:
    Player(Camera& camera)
    : mCamera{camera}
    {}

    void onGUI() override {
        GameObject::onGUI();
        if (ImGui::Button(inputEnabled ? "Disable input" : "Enable input")) {
            inputEnabled = !inputEnabled;
        }
    }

    void ready() override {
        GameObject::ready();
        runForward = gResources.getAnimation("run_forward.fbx");
        runBackwards = gResources.getAnimation("run_backwards.fbx");
        strafeRight = gResources.getAnimation("strafe_right.fbx");
        strafeLeft = gResources.getAnimation("strafe_left.fbx");

        mCamera.getTransform() = transform;
        mCamera.getTransform().position -= -5.0f * transform.front() + transform.up();
    }

    void update(Duration dt) override {
        GameObject::update(dt);

        velo = {0, 0, 0}; 

        if (inputEnabled) {
            if (InputManager::instance().keyPressed(GLFW_KEY_W)) {
                auto front = transform.front();
                velo.x += front.x;
                velo.z += front.z;
                animation = runForward;
            }
            else if (InputManager::instance().keyPressed(GLFW_KEY_S)) {
                auto front = transform.front();
                velo.x -= front.x;
                velo.z -= front.z;
                animation = runBackwards;
            }

            if (InputManager::instance().keyPressed(GLFW_KEY_A)) {
                auto right = transform.right();
                velo.x -= right.x;
                velo.z -= right.z;
                animation = strafeLeft;
            }
            else if (InputManager::instance().keyPressed(GLFW_KEY_D)) {
                auto right = transform.right();
                velo.x += right.x;
                velo.z += right.z;
                animation = strafeRight;
            }

            if (glm::length(velo) > std::numeric_limits<float>::epsilon()) {
                velo = speed * glm::normalize(velo);
                transform.position += dt.count() * velo;
            } else {
                animation = &skinnedModel->model.animations[0];
            }


            const auto mousePos = InputManager::instance().mousePos();
            const auto delta = mousePos - prevMousePos;
            prevMousePos = mousePos;

            auto yaw = glm::angleAxis(-sensitivity * delta.x, worldUp);
            transform.rotation = yaw * transform.rotation;

            auto pitch = glm::angleAxis(-sensitivity * delta.y, transform.right()); 

            Transform t = mCamera.getTransform();
            t.rotation = pitch * yaw * t.rotation;
            t.position = transform.position - 5.0f * t.front() + t.up();

            mCamera.getTransform() = t;

            /*
            mCamera.getTransform() = transform;
            mCamera.getTransform().rotation = pitch * mCamera.getTransform().rotation;
            mCamera.getTransform().position += -5.0f * transform.front() + transform.up();
            */
        }
    }

    Camera& mCamera;

private:
    glm::vec3 velo;
    glm::vec2 prevMousePos;
    float speed{5.0f};
    float sensitivity{0.001f};
    bool inputEnabled{false};
    const Animation* runForward;
    const Animation* runBackwards;
    const Animation* strafeRight;
    const Animation* strafeLeft;
};


#endif
