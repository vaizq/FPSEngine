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
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    Skeleton skeleton;
    vector<BoneInfo> bones;
    map<string, unsigned> boneIDs; // boneName -> boneIDs
    vector<Mesh>    meshes;
    glm::mat4 rootTransform{1.0f};
    std::vector<Animation> animations;
    string directory;
    bool gammaCorrection;

    Model() = default;

    // constructor, expects a filepath to a 3D model.
    explicit Model(string const &path, bool gamma = false, bool flipVertically = false)
    : gammaCorrection(gamma)
    {
        stbi_set_flip_vertically_on_load(flipVertically);
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void draw(Shader &shader)
    {
        glm::mat4 boneMatrices[200];

        for (int i = 0; i < skeleton.size(); i++) {
            const Joint& j = skeleton[i];
            if (boneIDs.contains(j.name)) {
                const unsigned id = boneIDs[j.name];
                const BoneInfo& bone = bones[id];
                boneMatrices[id] = jointTransform(i) * bone.offsetMatrix;
            }
        }

        shader.setMat4("finalBoneMatrices", boneMatrices[0], 200);

        for(auto& mesh : meshes) {
            mesh.draw(shader);
        }
    }

private:
    map<string, bool> necessaryNodes;
    glm::mat4 jointTransform(unsigned index);
    glm::mat4 jointFinalTransform(unsigned index);

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path);

    void findNecessaryNodes(aiNode *node, const aiScene *scene);
    
    void collectBones(const aiScene *scene);

    void processSkeleton(aiNode *node, const aiScene *scene, unsigned parentIndex);

    void loadAnimations(const aiScene *scene);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};


#endif //FPSFROMSCRATCH_MODEL_H
