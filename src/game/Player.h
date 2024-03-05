//
// Created by vaige on 27.2.2024.
//

#ifndef FPSFROMSCRATCH_PLAYER_H
#define FPSFROMSCRATCH_PLAYER_H


#include "../core/Model.h"
#include "../core/Shader.h"
#include "../core/Camera.h"
#include "../core/Util.h"
#include <imgui.h>

class Player
{
public:
    Player()
    :   mCamera{},
        mGun{Util::getAssetPath("models/ak47/ak47.obj"), true, false}
    {}

    void onGui()
    {
        ImGui::SliderAngle("YAW", &rotationYaw);
        ImGui::SliderAngle("PITCH", &rotationPitch);
        ImGui::SliderFloat3("Position", &pos[0], -10.f, 10.f);
        gunTranslation = glm::mat4{1};
        gunTranslation = glm::translate(gunTranslation, pos);
        gunTranslation = glm::rotate(gunTranslation, rotationYaw, glm::vec3{0.0f, 1.0f, 0.0f});
        gunTranslation = glm::rotate(gunTranslation, rotationPitch, glm::vec3{1.0f, 0.0f, 0.0f});
    }

    void draw(Shader& shader)
    {
        glm::mat4 view = mCamera.getViewMatrix();
        shader.setMat4("view", view);
        shader.setMat4("model", gunTranslation);
        mGun.draw(shader);
    }
private:
    glm::vec3 pos{0.13f, -0.234f, -0.519f};
    float rotationYaw{glm::radians(-80.f)};
    float rotationPitch{glm::radians(0.f)};
    glm::mat4 gunTranslation{};
    Camera mCamera;
    Model mGun;
};

#endif //FPSFROMSCRATCH_PLAYER_H
