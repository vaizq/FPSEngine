#include "SkinnedModel.hpp"
#include "Util.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>


void processNode(const aiScene* scene, const aiNode* node);


SkinnedModel* SkinnedModel::loadFromFile(const char* filepath) {
    // read file via ASSIMP
    Assimp::Importer importer;
    //aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    const aiScene* scene = importer.ReadFile(filepath, 
                                             aiProcess_LimitBoneWeights |
                                             aiProcess_Triangulate | 
                                             aiProcess_GenSmoothNormals | 
                                             aiProcess_FlipUVs |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_CalcTangentSpace);

    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        return nullptr;
    }

    auto model = new SkinnedModel{};

    processNode(scene, scene->mRootNode);

    return model;
}

void processNode(const aiScene* scene, const aiNode* node) {

    for(unsigned i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        for (unsigned j = 0; j < mesh->mNumBones; j++) {}

        const bool hasTangentsAndBitangents = mesh->HasTangentsAndBitangents();

        for (unsigned j = 0; j < mesh->mNumVertices; j++) {
            SkinnedVertex v;
            v.position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
            v.normal = glm::normalize(glm::vec3{mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z});
        }

        for (unsigned j = 0; j < mesh->mNumFaces; j++) {
            for (unsigned k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
            }
        }
    }

    for (unsigned i = 0; i < node->mNumChildren; i++) {
        processNode(scene, node->mChildren[i]);
    }
}

