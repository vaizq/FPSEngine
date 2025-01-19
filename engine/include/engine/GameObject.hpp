//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_GAMEOBJECT_H
#define FPSFROMSCRATCH_GAMEOBJECT_H

#include "Shader.hpp"
#include "Model.hpp"
#include <imgui.h>
#include "Transform.hpp"
#include <nlohmann/json.hpp>
#include "BoundingVolume.hpp"
#include "MyGui.hpp"
#include "SkinnedModel.hpp"
#include "ResourceManager.hpp"

enum class AnimationState {
    BindPose,
    Animate,
    Manual
};

struct GameObject
{
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<float>;

    std::string name{};
    GameObject* parent;
    Model* model{};
    std::unique_ptr<SkinnedModel> skinnedModel{};
    Transform transform;
    BoundingVolume bounds;
    std::vector<std::unique_ptr<GameObject>> children;
    bool renderModel{true};
    AnimationState animState{AnimationState::Animate};
    double animTime{0};
    const Animation* animation{};

    virtual ~GameObject() = default;

    Transform worldTransform()
    {
        GameObject* scene = this;
        auto modelMatrix = scene->transform.modelMatrix();

        while (scene->parent != nullptr) {
            scene = scene->parent;
            modelMatrix = scene->transform.modelMatrix() * modelMatrix;
        }

        return Transform{modelMatrix};
    }

    // ready is called just before the first frame when all game objects have been initialized
    virtual void ready() {
        if (skinnedModel && !skinnedModel->model.animations.empty()) {
            animation = &skinnedModel->model.animations[0];
        }
    }

    // Shutdown is called after the last frame before any of the game objects have been deleted
    virtual void shutdown() {}

    virtual void onGUI()
    {
        MyGui::InputTransform("LocalTransform", transform);
        MyGui::InputTransform("BoundsTransform", bounds.transform);

        if (ImGui::Button("toggleAnimation")) {
            animState = (animState == AnimationState::Animate) ? AnimationState::Manual : AnimationState::Animate;
        }

        if (ImGui::Button("toBindPose")) {
            animState = AnimationState::BindPose;
        }

        if (ImGui::Button(renderModel ? "Dont render" : "Render")) {
            renderModel = !renderModel;
        }

        if (animation) {
            float t = animTime;
            ImGui::SliderFloat("animation (t)", &t, 0, animation->duration);
            animTime = t;

            if (ImGui::BeginListBox("animations")) {
                if (ImGui::Selectable("default")) {
                        this->animation = &skinnedModel->model.animations[0];
                }

                for (const auto& animation : gResources.getAnimations()) {
                    if (ImGui::Selectable(animation.name.c_str())) {
                        this->animation = &animation;
                    }
                }
                ImGui::EndListBox();
            }
        }

        std::visit([this](auto&& shape) {
            using T = std::decay_t<decltype(shape)>;
            if constexpr (std::is_same_v<T, Sphere>) {
                ImGui::DragFloat("radius", &shape.radius);
            }
        }, bounds.shape);
    }

    virtual void update(std::chrono::duration<float> dt) {
        if (animState == AnimationState::Animate && animation) {
            animTime = animation->update(animTime, dt.count());
        }

        if (animation) {
            animation->update(animTime, skinnedModel->skeleton);
        }
    }

    virtual void render(Shader& shader, const glm::mat4& parentTransform = glm::mat4{1.0f})
    {
        auto modelMatrix = parentTransform * transform.modelMatrix();
        const auto normalMatrix = glm::transpose(glm::inverse(modelMatrix));

        shader.setMat4("model", modelMatrix);
        shader.setMat3("normalMatrix", normalMatrix);

        if (renderModel) {
            if (model != nullptr) {
                model->draw(shader);
            } else if (skinnedModel != nullptr) {
                skinnedModel->draw(shader);
            }
        }

        for (auto& child : children) {
            child->render(shader, modelMatrix);
        }
    }

    // Apply functor to this and all the children recursively
    void forEach(std::function<void(GameObject&)>&& callable)
    {
        callable(*this);
        for (auto& child : children) {
            child->forEach(std::move(callable));
        }
    }

    // Apply functor that takes GameObject and it's parent transform to this and to every children recursively
    void forEach(std::function<void(GameObject& entity, const glm::mat4& parentTransform)>&& callable, const glm::mat4& parentTransform = glm::mat4{1.0f})
    {
        callable(*this, parentTransform);
        const auto totalTransform = parentTransform * transform.modelMatrix();
        for (auto& child : children) {
            child->forEach(std::move(callable), totalTransform);
        }
    }

    GameObject* findChildren(const std::string& childName)
    {
        if (this->name == childName) {
            return this;
        }

        GameObject* result{nullptr};

        for (auto& child : children) {
            result = child->findChildren(childName);
            if (result != nullptr) {
                return result;
            }
        }

        return result;
    }

    GameObject* findGameObject(const std::string& objectName) {
        // First find the scene, then from it's children
        auto scene = this;
        while (scene->parent != nullptr) {
            scene = scene->parent;
        }

        return scene->findChildren(objectName);
    }

    // Give object type and name and get a pointer to that object or nullptr if not found
    template<typename T>
    T* findGameObject(const std::string& objectName)
    {
        return dynamic_cast<T*>(findGameObject(objectName));
    }

    GameObject* getScene()
    {
        auto scene = this;
        while (scene->parent != nullptr) {
            scene = scene->parent;
        }
        return scene;
    }

    static nlohmann::json serialize(const GameObject& entity);
    static void deserialize(const nlohmann::json& j, GameObject& obj);
};


#endif //FPSFROMSCRATCH_GAMEOBJECT_H
