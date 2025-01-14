#ifndef SKINNED_MODEL_HPP
#define SKINNED_MODEL_HPP

#include "Model.hpp"

struct SkinnedMesh {
    std::map<std::string, unsigned> boneIDs;
    std::vector<Mesh> meshes;
};

struct SkinnedModel {
    double animTime{0.0};
    Skeleton skeleton;
    Model& model;


    void draw(Shader& shader) {
        glm::mat4 boneMatrices[200];

        model.animations[0].update(animTime, skeleton);

        for (int i = 0; i < skeleton.size(); i++) {
            const Joint& j = skeleton[i];
            if (model.boneIDs.contains(j.name)) {
                const unsigned id = model.boneIDs[j.name];
                const BoneInfo& bone = model.bones[id];
                boneMatrices[id] = j.finalTransform * bone.offsetMatrix;
            }
        }

        shader.setMat4("finalBoneMatrices", boneMatrices[0], 200);

        for(auto& mesh : model.meshes) {
            mesh.draw(shader);
        }

    }
};


#endif
