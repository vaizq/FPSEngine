//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_MODEL_H
#define FPSFROMSCRATCH_MODEL_H

#include "Util.hpp"
#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include <format>
#include "Animation.hpp"

using namespace std;

class Model
{
public:
    // model data
    Skeleton skeleton;
    vector<Mesh>    meshes;
    std::vector<Animation> animations;

    Model() = default;

    // Constructor prepares model to be loaded to GPU
    explicit Model(string const &path, bool gamma = false, bool flipVertically = false)
    {
        stbi_set_flip_vertically_on_load(flipVertically);
        loadModel(path);
    }

    void load() {
        for (auto& mesh : meshes) {
            mesh.load();
        }
    }

    void draw(Shader &shader)
    {
        glm::mat4 boneMatrices[200];

        for (int i = 0; i < skeleton.joints.size(); i++) {
            const Joint& j = skeleton.joints[i];
            if (skeleton.boneIDs.contains(j.name)) {
                const unsigned id = skeleton.boneIDs[j.name];
                const BoneInfo& bone = skeleton.bones[id];
                boneMatrices[id] = jointTransform(i) * bone.offsetMatrix;
            }
        }

        shader.setMat4("finalBoneMatrices", boneMatrices[0], 200);

        for(auto& mesh : meshes) {
            mesh.draw(shader);
        }
    }

private:
    string directory;

    glm::mat4 jointTransform(unsigned index);
    void loadModel(string const &path);
};


#endif //FPSFROMSCRATCH_MODEL_H
