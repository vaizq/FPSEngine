#ifndef DRONE_H
#define DRONE_H

#include "engine/GameObject.hpp"
#include "engine/Camera.hpp"
#include "engine/InputManager.hpp"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>


class Drone : public GameObject {
public:
    Drone(Camera& camera)
    : camera{camera}
    {
        prevMousePos = InputManager::instance().mousePos();
    }

    void onGUI() override {
        GameObject::onGUI();
        if (ImGui::Button(inputEnabled ? "Disable input" : "Enable input")) {
            inputEnabled = !inputEnabled;
        }
    }

    void update(Duration dt) override {
        velo = {0, 0, 0}; 

        if (inputEnabled) {
            if (InputManager::instance().keyPressed(GLFW_KEY_W)) {
                auto front = transform.front();
                velo += front;
            }
            else if (InputManager::instance().keyPressed(GLFW_KEY_S)) {
                auto front = transform.front();
                velo -= front;
            }

            if (InputManager::instance().keyPressed(GLFW_KEY_A)) {
                auto right = transform.right();
                velo -= right;
            }
            else if (InputManager::instance().keyPressed(GLFW_KEY_D)) {
                auto right = transform.right();
                velo += right;
            }

            if (glm::length(velo) > std::numeric_limits<float>::epsilon()) {
                velo = speed * glm::normalize(velo);
                transform.position += dt.count() * velo;
            }


            const auto mousePos = InputManager::instance().mousePos();
            const auto delta = mousePos - prevMousePos;
            prevMousePos = mousePos;

            auto yaw = glm::angleAxis(-sensitivity * delta.x, worldUp);
            transform.rotation = yaw * transform.rotation;

            auto pitch = glm::angleAxis(-sensitivity * delta.y, transform.right());
            transform.rotation = pitch * transform.rotation;

            camera.getTransform() = transform;
        }
    }

    void enableInput() {
        inputEnabled = true;
        prevMousePos = InputManager::instance().mousePos();
    }

    void disableInput() {
        inputEnabled = false;
        prevMousePos = InputManager::instance().mousePos();
    }

private:
    Camera& camera;
    glm::vec3 velo;
    glm::vec2 prevMousePos;
    float speed{5.0f};
    float sensitivity{0.001f};
    bool inputEnabled{false};
};


#endif
