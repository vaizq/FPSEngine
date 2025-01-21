//
// Created by vaige on 19.2.2024.
//

#include "Model.hpp"
#include "Mesh.hpp"
#include <assimp/Exporter.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include "AssetImporter.hpp"
#include "engine/Util.hpp"
#include "Transform.hpp"


glm::mat4 aiToGlmMat(const aiMatrix4x4& from) {
    glm::mat4 to;


    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

void printSkeleton(const Skeleton& skeleton, size_t index = 0, int numParents = 0) {
    std::string msg;
    for (int i = 0; i < numParents; i++) {
        msg += " ";
    }
    msg += skeleton.joints[index].name;

    printf("%s\n", msg.c_str());

    for (int i = 0; i < skeleton.joints.size(); i++) {
        if (skeleton.joints[i].parent == index) {
            printSkeleton(skeleton, i, numParents + 1);
        }
    }
}

glm::mat4 Model::jointTransform(unsigned index) {
    const Joint& j = skeleton.joints[index];
    if (index == 0) {
        return j.transformation;
    } else {
        return jointTransform(j.parent) * j.transformation;
    }
}

void Model::loadModel(string const &path)
{
    std::cout << "Load model from " << path << std::endl;
    // read file via ASSIMP
    Assimp::Importer importer;

    //aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    const aiScene* scene = importer.ReadFile(path, 
                                             aiProcess_LimitBoneWeights |
                                             aiProcess_Triangulate | 
                                             aiProcess_GenSmoothNormals | 
                                             aiProcess_FlipUVs |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_PopulateArmatureData |
                                             aiProcess_ValidateDataStructure |
                                             aiProcess_FindInvalidData);

    // check for errors
    if(!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    try {
        skeleton = AssetImporter::loadSkeleton(scene);
        animations = AssetImporter::loadAnimations(scene);
    } catch (const std::exception& e) {
        printf("%s\n", e.what());
    }

    meshes = AssetImporter::loadMeshes(scene, skeleton, directory);
}
