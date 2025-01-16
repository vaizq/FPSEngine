#include "AssetImporter.hpp"
#include "assimp/Importer.hpp"
#include <assimp/postprocess.h>
#include "Util.hpp"


void findNecessaryNodes(const aiScene *scene, const aiNode* node, std::map<std::string, bool>& necessaryNodes);
void collectBones(const aiScene *scene, Skeleton& skeleton);
void processSkeleton(const aiScene *scene, const aiNode* node, Skeleton& skeleton, std::map<std::string, bool>& necessaryNodes, unsigned parentIndex);


std::vector<Animation> AssetImporter::loadAnimations(const aiScene* scene) {
    std::vector<Animation> animations;

    for (int i = 0; i < scene->mNumAnimations; i++) {

        aiAnimation* aiAnim = scene->mAnimations[i];

        const double ticksPerSec = (aiAnim->mTicksPerSecond <= std::numeric_limits<double>::epsilon()) ? 1.0 : aiAnim->mTicksPerSecond;

        Animation animation {
            aiAnim->mName.C_Str(),
            aiAnim->mDuration / ticksPerSec
        };

        for (int j = 0; j < aiAnim->mNumChannels; j++) {

            aiNodeAnim* aiChan = aiAnim->mChannels[j];
            assert(aiChan->mPositionKeys != nullptr && aiChan->mRotationKeys != nullptr && aiChan->mScalingKeys != nullptr);
            AnimationChannel chan;

            for (int k = 0; k < aiChan->mNumPositionKeys; k++) {
                aiVectorKey key = aiChan->mPositionKeys[k]; 
                chan.positionKeys.emplace_back(
                    key.mTime / ticksPerSec,
                    glm::vec3{key.mValue.x, key.mValue.y, key.mValue.z}
                );
            }

            for (int k = 0; k < aiChan->mNumRotationKeys; k++) {
                aiQuatKey key = aiChan->mRotationKeys[k]; 
                chan.rotationKeys.emplace_back(
                    key.mTime / ticksPerSec,
                    glm::quat{key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z}
                );
            }

            for (int k = 0; k < aiChan->mNumScalingKeys; k++) {
                aiVectorKey key = aiChan->mScalingKeys[k]; 
                chan.scalingKeys.emplace_back(
                    key.mTime / ticksPerSec,
                    glm::vec3{key.mValue.x, key.mValue.y, key.mValue.z}
                );
            }

            animation.channels[aiChan->mNodeName.C_Str()] = std::move(chan);
        }

        animations.push_back(animation);
    }

    return animations;
}

Skeleton AssetImporter::loadSkeleton(const aiScene* scene) {
    std::map<std::string, bool> necessaryNodes;
    findNecessaryNodes(scene, scene->mRootNode, necessaryNodes);

    Skeleton skeleton;
    collectBones(scene, skeleton);
    processSkeleton(scene, scene->mRootNode, skeleton, necessaryNodes, 0xFF);
    return skeleton;
}

void findNecessaryNodes(const aiScene *scene, const aiNode* node, std::map<std::string, bool>& necessaryNodes) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        for (int j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[j];
            for(aiNode* tmpNode = bone->mNode; 
                tmpNode != nullptr && tmpNode != node && tmpNode != node->mParent; 
                tmpNode = tmpNode->mParent) {
                necessaryNodes[tmpNode->mName.C_Str()] = true;
            }
        }
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        findNecessaryNodes(scene, node->mChildren[i], necessaryNodes);
    }
}

void collectBones(const aiScene *scene, Skeleton& skeleton) {
    for (unsigned i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[j];
            if (!skeleton.boneIDs.contains(bone->mName.C_Str())) {
                skeleton.bones.emplace_back(bone->mName.C_Str(), Util::aiMatrix4x4ToGlmMat4(bone->mOffsetMatrix));
                unsigned index = skeleton.bones.size() - 1;
                skeleton.boneIDs[bone->mName.C_Str()] = index;
            }
        }
    }
}

void processSkeleton(const aiScene *scene, const aiNode* node, Skeleton& skeleton, std::map<std::string, bool>& necessaryNodes, unsigned parentIndex) {
    if (necessaryNodes[node->mName.C_Str()]) {
        Joint j{node->mName.C_Str(), parentIndex, Util::aiMatrix4x4ToGlmMat4(node->mTransformation)};
        skeleton.joints.push_back(j);
        const unsigned newParentIndex = skeleton.joints.size() - 1;

        for(unsigned i = 0; i < node->mNumChildren; i++)
        {
            aiNode* child = node->mChildren[i];
            if (necessaryNodes[child->mName.C_Str()]) {
                processSkeleton(scene, child, skeleton, necessaryNodes, newParentIndex);
            }
        }
    } else {
        for(unsigned i = 0; i < node->mNumChildren; i++)
        {
            aiNode* child = node->mChildren[i];
            if (necessaryNodes[child->mName.C_Str()]) {
                processSkeleton(scene, child, skeleton, necessaryNodes, parentIndex);
                return;
            }
        }
    }
}
