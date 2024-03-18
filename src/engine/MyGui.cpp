//
// Created by vaige on 4.3.2024.
//

#include "MyGui.h"

#include <imgui.h>
#include <numbers>


void MyGui::InputTransform(const char* label, Transform& transform, float speed)
{
    ImGui::PushID(label);

    if (ImGui::Button("Reset")) {
        transform = Transform();
    }
    ImGui::DragFloat3("Position", &transform.position[0], speed);
    auto orientation = glm::degrees(glm::eulerAngles(transform.rotation));
    ImGui::DragFloat3("Orientation", &orientation[0], speed);
    ImGui::DragFloat3("Scale", &transform.scale[0], speed);

    ImGui::PopID();
}
