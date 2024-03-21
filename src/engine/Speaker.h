//
// Created by vaige on 21.3.2024.
//

#ifndef FPSFROMSCRATCH_SPEAKER_H
#define FPSFROMSCRATCH_SPEAKER_H

#include "GameObject.h"
#include "AudioSource.h"
#include "Geometry.h"
#include "ResourceManager.h"
#include <imgui_stdlib.h>

class Speaker : public GameObject
{
public:

    void update(Duration dt) override
    {
        mAudio.setTransform(worldTransform());
    }

    void onGUI() override
    {
        GameObject::onGUI();
        if(ImGui::Button(mRender ? "Dont Render" : "Render")) {
            mRender = !mRender;
        }

        ImGui::InputText("Audio name", &mAudioName);

        if (ImGui::Button(mAudio.isPlaying() ? "Stop" : "Play")) {
            if (mAudio.isPlaying()) {
                mAudio.stopAudio();
            }
            else {
                try {
                    mAudio.playAudio(mAudioName);
                }
                catch(const std::exception& e) {
                    std::cerr << "Invalid audio file!\n";
                }
            }
        }
    }

    void render(Shader& shader, const glm::mat4& parentTransform) override
    {
        if (mRender) {
            auto &colorShader = ResourceManager::instance().getShader("color");
            colorShader.use();
            colorShader.setMat4("view", ResourceManager::instance().view);
            colorShader.setMat4("projection", ResourceManager::instance().projection);
            colorShader.setMat4("model", parentTransform * transform.modelMatrix());
            colorShader.setVec3("color", Util::green);
            mMesh.draw(colorShader);
            shader.use();
        }
    }

private:
    AudioSource mAudio;
    std::string mAudioName;
    Mesh mMesh{Geometry::makeBox()};
    bool mRender{false};
};


#endif //FPSFROMSCRATCH_SPEAKER_H
