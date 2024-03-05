//
// Created by vaige on 4.3.2024.
//

#include "MyGui.h"

#include <imgui.h>


void MyGui::InputTransform(const char* label, Transform& transform, float speed)
{
    ImGui::PushID(label);

    ImGui::DragFloat3("Position", &transform.position[0], speed);
    ImGui::SliderAngle("Yaw", &transform.yaw);
    ImGui::SliderAngle("Pitch", &transform.pitch);
    ImGui::DragFloat("Scale", &transform.scale, speed);

    ImGui::PopID();
}
