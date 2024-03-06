//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_GAMEOBJECT_H
#define FPSFROMSCRATCH_GAMEOBJECT_H

#include "Shader.h"
#include "Model.h"
#include <imgui.h>
#include "Transform.h"
#include <nlohmann/json.hpp>
#include "BoundingVolume.h"
#include "MyGui.h"
#include "Camera.h"


struct GameObject
{
    using Duration = std::chrono::duration<float>;

    std::string name{};
    GameObject* parent;
    Model* model{};
    Transform transform;
    BoundingVolume bounds;
    std::vector<std::unique_ptr<GameObject>> children;

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

    virtual void onGUI()
    {
        ImGui::SeparatorText("Initial transformation");
        MyGui::InputTransform("initial", transform);

        ImGui::SeparatorText("BoundingVolume");
        MyGui::InputTransform("bounds", bounds.transform);

        std::visit([this](auto&& shape) {
            using T = std::decay_t<decltype(shape)>;
            if constexpr (std::is_same_v<T, Sphere>) {
                ImGui::DragFloat("radius", &shape.radius);
            }

        }, bounds.shape);
    }

    virtual void update(std::chrono::duration<float> dt) {}

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

    GameObject* findChildren(const std::string& name)
    {
        if (this->name == name) {
            return this;
        }

        GameObject* result{nullptr};

        for (auto& child : children) {
            result = child->findChildren(name);
            if (result != nullptr) {
                return result;
            }
        }

        return result;
    }

    static nlohmann::json serialize(const GameObject& entity);
    static void deserialize(const nlohmann::json& j, GameObject& obj);
};


#endif //FPSFROMSCRATCH_GAMEOBJECT_H