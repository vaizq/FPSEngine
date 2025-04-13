#ifndef SKINNED_MODEL_HPP
#define SKINNED_MODEL_HPP

#include "Model.hpp"

struct SkinnedMesh {
    std::map<std::string, unsigned> boneIDs;
    std::vector<Mesh> meshes;
};

struct SkinnedModel {
    Skeleton skeleton;
    Model& model;

    void draw(Shader& shader) {
        glm::mat4 boneMatrices[200];

        for (const auto& [_, id] : skeleton.boneIDs) {
            const auto& bone = skeleton.bones[id];
            boneMatrices[id] = bone.finalTransform;
        }

        shader.setMat4("finalBoneMatrices", boneMatrices[0], 200);

        for(auto& mesh : model.meshes) {
            mesh.draw(shader);
        }

    }
};


#endif
