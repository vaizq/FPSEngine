//
// Created by vaige on 13.3.2024.
//

#ifndef FPSFROMSCRATCH_TERRAIN_H
#define FPSFROMSCRATCH_TERRAIN_H

#include "GameObject.hpp"
#include "Renderer.hpp"


class Terrain : public GameObject
{
public:
    Terrain();
    ~Terrain() override;

    void render(Shader& _, const glm::mat4& parentTransform = glm::mat4{1.0f}) override {
        ShaderHandle handle(Renderer::ShaderID::Basic);

        auto modelMatrix = parentTransform * transform.modelMatrix();
        const auto normalMatrix = glm::transpose(glm::inverse(modelMatrix));

        handle.shader().setMat4("projection", gRenderer.getProjection());
        handle.shader().setMat4("view", gRenderer.getView());

        handle.shader().setMat4("model", modelMatrix);
        handle.shader().setMat3("normalMatrix", normalMatrix);

        for (auto& mesh : model->meshes) {
            mesh.draw(handle.shader());
        }
    }
};


#endif //FPSFROMSCRATCH_TERRAIN_H
