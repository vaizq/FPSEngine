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
    Model* model{};
    std::string name{};
    glm::mat4 translation{1.0f};
    glm::vec3 pos{};
    glm::vec3 scale{};
    float yaw{};
    float pitch{};

    void update() {
        ImGui::PushID(this);
        ImGui::SeparatorText(name.c_str());
        ImGui::SliderAngle("YAW", &yaw);
        ImGui::SliderAngle("PITCH", &pitch);
        ImGui::DragFloat3("Position", &pos[0], 0.1f, -100.f, 100.f);
        ImGui::DragFloat3("Scale", &scale[0], 0.1f, 0.0f, 10.0f);
        translation = glm::mat4{1};
        translation = glm::translate(translation, pos);
        translation = glm::rotate(translation, yaw, glm::vec3{0.0f, 1.0f, 0.0f});
        translation = glm::rotate(translation, pitch, glm::vec3{1.0f, 0.0f, 0.0f});
        translation = glm::scale(translation, scale);
        ImGui::PopID();
    }

    void draw(Shader& shader) {
        shader.setMat4("model", translation);
        model->draw(shader);
    }
};


#endif //FPSFROMSCRATCH_ENTITY_H
