#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "engine/GameObject.hpp"
#include "engine/Camera.hpp"
#include "engine/InputManager.hpp"
#include "engine/ResourceManager.hpp"
#include <algorithm>
#include <glm/ext/quaternion_geometric.hpp>
#include <limits>
#include "engine/ResourceManager.hpp"


enum class Movement {
    Idle,
    Running,
    Jumping,
    InAir,
    Landing
};

static std::string to_string(Movement movement) {
    switch (movement) {
        case Movement::Idle:
            return "Idle";
        case Movement::Running:
            return "Running";
        case Movement::Jumping:
            return "Jumping";
        case Movement::InAir:
            return "InAir";
        case Movement::Landing:
            return "Landing";
        default:
            return "Default";
    }
}

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

        if (ImGui::Button("reset camera")) {
            resetCamera();
        }

        ImGui::DragFloat3("velocity (m/s)", &velo[0], 0.1f);

        ImGui::Text("state: %s", to_string(state).c_str());
    }

    void ready() override {
        GameObject::ready();
        idle = gResources.getAnimation("aim_idle.fbx").get();
        runForward = gResources.getAnimation("run_forward.fbx").get();
        runBackwards = gResources.getAnimation("run_backwards.fbx").get();
        strafeRight = gResources.getAnimation("strafe_right.fbx").get();
        strafeLeft = gResources.getAnimation("strafe_left.fbx").get();
        jumpUp = gResources.getAnimation("jump_up.fbx").get();
        jumpDown = gResources.getAnimation("jump_down.fbx").get();

        resetCamera();
    }

    void resetCamera() {
        mCamera.getTransform() = transform;
        mCamera.getTransform().position -= -5.0f * transform.front() + transform.up();
    }

    void update(const Duration dt) override {
        GameObject::update(dt);

        if (inputEnabled) {
            zoom = std::max(InputManager::instance().scrollOffset(), 0.0f);

            glm::vec3 planarVelo{0.0f};
            if (InputManager::instance().keyPressed(GLFW_KEY_W) && (state == Movement::Idle || state == Movement::Running)) {
                auto front = transform.front();
                planarVelo.x += front.x;
                planarVelo.z += front.z;
                animation = runForward;
            }
            else if (InputManager::instance().keyPressed(GLFW_KEY_S) && (state == Movement::Idle || state == Movement::Running)) {
                auto front = transform.front();
                planarVelo.x -= front.x;
                planarVelo.z -= front.z;
                animation = runBackwards;
            }

            if (InputManager::instance().keyPressed(GLFW_KEY_A) && (state == Movement::Idle || state == Movement::Running)) {
                auto right = transform.right();
                planarVelo.x -= right.x;
                planarVelo.z -= right.z;
                animation = strafeLeft;
            }
            else if (InputManager::instance().keyPressed(GLFW_KEY_D) && (state == Movement::Idle || state == Movement::Running)) {
                auto right = transform.right();
                planarVelo.x += right.x;
                planarVelo.z += right.z;
                animation = strafeRight;
            }

            if (InputManager::instance().keyPressed(GLFW_KEY_SPACE) && (state == Movement::Idle || state == Movement::Running)) {
                state = Movement::Jumping;
                animation = jumpUp;
                animTime = 0;
                veloAtJump = velo;
            }

            if (state != Movement::Jumping && glm::length(planarVelo) > std::numeric_limits<float>::epsilon()) {
                state = Movement::Running;
            } else if (state == Movement::Running) {
                state = Movement::Idle;
            }

            switch (state) {
                case Movement::Running: {
                    planarVelo = speed * glm::normalize(planarVelo);
                    velo.x = planarVelo.x;
                    velo.z = planarVelo.z;
                    break;
                }
                case Movement::Idle: {
                    velo.x = 0;
                    velo.z = 0;
                    animation = idle;
                    break;
                }
                case Movement::Jumping: {
                    velo.x = veloAtJump.x / 4;
                    velo.z = veloAtJump.z / 4;

                    if (animTime > 0.4 * animation->duration) {
                        state = Movement::InAir;
                        velo.y = 3.5f;
                    }
                    break;
                }
                case Movement::InAir: {
                    velo.x = veloAtJump.x / 2;
                    velo.z = veloAtJump.z / 2;

                    velo.y += dt.count() * Util::gravity.y;

                    if (animTime > 0.95 * animation->duration) {
                        animTime = 0.95 * animation->duration;
                    }

                    if (velo.y < 0.0f) {
                        if (animation != jumpDown) {
                            animTime = 0.0;
                            animation = jumpDown;
                        }
                    }

                    if (transform.position.y <= 0.0f) {
                        transform.position.y = 0.0f;
                        velo.y = 0;
                        state = Movement::Landing;
                    }

                    break;
                }
                case Movement::Landing: {
                    velo.x = veloAtJump.x / 4;
                    velo.z = veloAtJump.z / 4;

                    if (animTime > 0.95 * jumpDown->duration) {
                        velo.x = veloAtJump.x;
                        velo.y = veloAtJump.y;
                        state = Movement::Idle;
                    } 
                    break;
                }
                default: {
                    break;
                }
            }

            if (glm::length(velo) > std::numeric_limits<float>::epsilon()) {
                transform.position += dt.count() * velo;
            } else if (state == Movement::Running) {
                state = Movement::Idle;
            }

            // Rotation and camera updation
            const auto mousePos = InputManager::instance().mousePos();
            const auto delta = mousePos - prevMousePos;
            prevMousePos = mousePos;

            auto yaw = glm::angleAxis(-sensitivity * delta.x, worldUp);
            transform.rotation = yaw * transform.rotation;

            auto pitch = glm::angleAxis(-sensitivity * delta.y, transform.right()); 

            Transform t = mCamera.getTransform();
            t.rotation = pitch * yaw * t.rotation;
            t.position = transform.position - (5.0f + zoom) * t.front() + t.up();

            mCamera.getTransform() = t;
        }
    }


private:
    glm::vec3 velo;
    glm::vec3 veloAtJump;
    glm::vec2 prevMousePos;
    float speed{5.0f};
    float sensitivity{0.001f};
    bool inputEnabled{false};
    Camera& mCamera;
    const Animation* idle;
    const Animation* runForward;
    const Animation* runBackwards;
    const Animation* strafeRight;
    const Animation* strafeLeft;
    const Animation* jumpUp;
    const Animation* jumpDown;
    Movement state{Movement::Idle};
    float zoom = 0.0f;
};


#endif
