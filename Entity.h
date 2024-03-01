//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_ENTITY_H
#define FPSFROMSCRATCH_ENTITY_H

#include "Shader.h"
#include "Model.h"
#include <imgui.h>


struct Entity
{
    std::string name{};
    Model* model{};
    glm::vec3 position{};
    float yaw{};
    float pitch{};
    float scale{1.f};

    void onGUI()
    {
        ImGui::DragFloat3("Position", &position[0], 0.1f);
        ImGui::SliderAngle("Yaw", &yaw);
        ImGui::SliderAngle("Pitch", &pitch);
        ImGui::DragFloat("Scale", &scale, 0.1f);
    }

    [[nodiscard]] glm::mat4 modelViewMatrix() const
    {
        glm::mat4 modelMatrix{1.0f};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, yaw, glm::vec3(0.f, 1.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, pitch, glm::vec3(1.f, 0.f, 0.f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3{scale});
        return modelMatrix;
    }
};

struct Scene
{
    using Ptr = std::unique_ptr<Scene>;
    Entity entity;
    std::vector<Scene::Ptr> children;

    void forEach(std::function<void(Entity&)>&& callable) {
        callable(entity);
        for (auto& child : children) {
            child->forEach(std::move(callable));
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
