//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_ENTITY_H
#define FPSFROMSCRATCH_ENTITY_H

#include "Shader.h"
#include "Model.h"
#include <imgui.h>
#include "Transform.h"
#include <nlohmann/json.hpp>
#include "BoundingVolume.h"
#include "MyGui.h"
#include "Camera.h"


struct Entity
{
    std::string name{};
    Model* model{};
    Camera* camera{};
    Transform initialTransform;
    Transform deltaTransform;
    BoundingVolume bounds;

    void onGUI()
    {
        ImGui::SeparatorText("Initial transformation");
        MyGui::InputTransform("initial", initialTransform);

        ImGui::SeparatorText("Delta transformation");
        MyGui::InputTransform("delta", deltaTransform);

        ImGui::SeparatorText("BoundingVolume");
        MyGui::InputTransform("bounds", bounds.transform);

        std::visit([this](auto&& shape) {
            using T = std::decay_t<decltype(shape)>;
            if constexpr (std::is_same_v<T, Sphere>) {
                ImGui::DragFloat("radius", &shape.radius);
            }

        }, bounds.shape);
    }

    [[nodiscard]] glm::mat4 modelViewMatrix() const
    {
        Transform totalTransform{initialTransform.position + deltaTransform.position,
                                 initialTransform.yaw + deltaTransform.yaw,
                                 initialTransform.pitch + deltaTransform.pitch,
                                 initialTransform.scale * deltaTransform.scale};
        return totalTransform.modelMatrix();
    }

    static nlohmann::json serialize(const Entity& entity);
    static Entity deserialize(const nlohmann::json& j);
};


struct Scene
{
    using Ptr = std::unique_ptr<Scene>;
    Entity entity;
    std::vector<Scene::Ptr> children;

    Entity* getEntity(const std::string& name)
    {
        if (entity.name == name) {
            return &entity;
        }
        else if(!children.empty()) {
            for (auto& child : children) {
                if (auto* e = child->getEntity(name); e != nullptr) {
                    return e;
                }
            }
        }

        return nullptr;
    }

    void forEach(std::function<void(Entity&)>&& callable) {
        callable(entity);
        for (auto& child : children) {
            child->forEach(std::move(callable));
        }
    }

    void forEach(std::function<void(Entity& entity, const glm::mat4& parentTransform)>&& callable, const glm::mat4& transform = glm::mat4{1.0f}) {

        callable(entity, transform);

        auto t2 = transform * entity.modelViewMatrix();

        for (auto& child : children) {
            child->forEach(std::move(callable), t2);
        }
    }

    void draw(Shader& shader, const glm::mat4& transform = glm::mat4{1.0f}) const
    {
        auto t2 = transform * entity.modelViewMatrix();
        shader.setMat4("model", t2);

        if (entity.model != nullptr) {
            entity.model->draw(shader);
        }

        for (auto& child : children)
        {
            child->draw(shader, t2);
        }
    }
};

#endif //FPSFROMSCRATCH_ENTITY_H
