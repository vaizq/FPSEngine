#include "AssetImporter.hpp"
#include "assimp/Importer.hpp"
#include <assimp/postprocess.h>
#include <stdexcept>
#include "Util.hpp"


struct MeshLoaderParams {
    const aiScene* scene;
    const Skeleton& skeleton;
    const std::string& directory;
    std::vector<Texture> textures_loaded;
};


void findNecessaryNodes(const aiScene *scene, const aiNode* node, std::map<std::string, bool>& necessaryNodes);
void collectBones(const aiScene *scene, Skeleton& skeleton);
const aiNode* findSkeletonRoot(const aiNode* node, std::map<std::string, bool>& necessaryNodes);
void processSkeleton(const aiNode* node, Skeleton& skeleton, std::map<std::string, bool>& necessaryNodes, unsigned parentIndex);
void processMesh(MeshLoaderParams& params, const aiMesh* mesh, std::vector<Mesh>& meshes);
void processMeshes(MeshLoaderParams& params, const aiNode* node, std::vector<Mesh>& meshes);
std::vector<Texture> loadMaterialTextures(MeshLoaderParams& params, aiMaterial *mat, aiTextureType type, std::string typeName);


std::vector<Mesh> AssetImporter::loadMeshes(const aiScene* scene, const Skeleton& skeleton, const std::string& directory) {
    std::vector<Mesh> meshes;
    MeshLoaderParams params{scene, skeleton, directory};
    processMeshes(params, scene->mRootNode, meshes);
    return meshes;
}

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

    const aiNode* skeletonRoot = findSkeletonRoot(scene->mRootNode, necessaryNodes);
    if (skeletonRoot == nullptr) {
        throw std::runtime_error("unable to find skeleton root");
    }

    processSkeleton(skeletonRoot, skeleton, necessaryNodes, 0xFF);

    return skeleton;
}

void findNecessaryNodes(const aiScene *scene, const aiNode* node, std::map<std::string, bool>& necessaryNodes) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        for (int j = 0; j < mesh->mNumBones; j++) {
            aiBone* bone = mesh->mBones[j];

            if (bone->mNode == nullptr) continue;

            for(aiNode* tmpNode = bone->mNode; 
                tmpNode != nullptr && 
                tmpNode != node && 
                tmpNode != node->mParent;
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

const aiNode* findSkeletonRoot(const aiNode* node, std::map<std::string, bool>& necessaryNodes) {
    if (necessaryNodes[node->mName.C_Str()]) {
        return node;
    } else {
        for (int i = 0; i < node->mNumChildren; i++) {
            const aiNode* child = node->mChildren[i];
            if (auto root = findSkeletonRoot(child, necessaryNodes); root != nullptr) {
                return root;
            }
        }
    }

    return nullptr;
}

void processSkeleton(const aiNode* node, Skeleton& skeleton, std::map<std::string, bool>& necessaryNodes, unsigned parentIndex) {
    Joint j{node->mName.C_Str(), parentIndex, Util::aiMatrix4x4ToGlmMat4(node->mTransformation)};
    skeleton.joints.push_back(j);
    const unsigned newParentIndex = skeleton.joints.size() - 1;

    for(unsigned i = 0; i < node->mNumChildren; i++)
    {
        aiNode* child = node->mChildren[i];
        if (necessaryNodes[child->mName.C_Str()]) {
            processSkeleton(child, skeleton, necessaryNodes, newParentIndex);
        }
    }
}

void processMeshes(MeshLoaderParams& params, const aiNode* node, std::vector<Mesh>& meshes) {
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = params.scene->mMeshes[node->mMeshes[i]];
        processMesh(params, mesh, meshes);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processMeshes(params, node->mChildren[i], meshes);
    }

}

void processMesh(MeshLoaderParams& params, const aiMesh* mesh, std::vector<Mesh>& meshes) {
    using namespace std;
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};

        auto pos = mesh->mVertices[i];
        vertex.position.x = pos.x;
        vertex.position.y = pos.y;
        vertex.position.z = pos.z;

        if (mesh->HasNormals())
        {
            auto normal = mesh->mNormals[i];
            vertex.normal.x = normal.x;
            vertex.normal.y = normal.y;
            vertex.normal.z = normal.z;
        }

        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vec;
            // tangent
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
            // bitangent
            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }
        else
            vertex.texCoord = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    for(unsigned int i = 0; i < mesh->mNumBones; ++i) {
        aiBone* bone = mesh->mBones[i];
        const unsigned boneId = params.skeleton.boneIDs.at(bone->mName.C_Str());

        for (unsigned j = 0; j < bone->mNumWeights; j++) {
            const unsigned vertexId = bone->mWeights[j].mVertexId;
            const float weight = bone->mWeights[j].mWeight;

            Vertex& v = vertices[vertexId];

            if (v.boneWeights.x <= 0.0f) {
                v.boneIDs.x = boneId;
                v.boneWeights.x = weight;
            } else if (v.boneWeights.y <= 0.0f) {
                v.boneIDs.y = boneId;
                v.boneWeights.y = weight;
            } else if (v.boneWeights.z <= 0.0f) {
                v.boneIDs.z = boneId;
                v.boneWeights.z = weight;
            } else if (v.boneWeights.w <= 0.0f) {
                v.boneIDs.w = boneId;
                v.boneWeights.w = weight;
            } else {
                break;
            }
        }
    }

    // Normalize boneweights
    constexpr float threshold = 0.05f;
    for (Vertex& v : vertices) {

        float totalWeight = 0.0f;

        for (int i = 0; i < 4; i++) {
            if (v.boneWeights[i] < threshold) {
                v.boneWeights[i] = 0.0f;
            } else {
                totalWeight += v.boneWeights[i];
            }
        }

        if (totalWeight > 0) {
            v.boneWeights /= totalWeight;
        }
    }

    // process materials
    aiMaterial* material = params.scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(params, material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(params, material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(params, material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(params, material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    meshes.emplace_back(std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<Texture> loadMaterialTextures(MeshLoaderParams& params, aiMaterial *mat, aiTextureType type, std::string typeName)
{
    using namespace std;
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(auto& loadedTexture : params.textures_loaded)
        {
            if(std::strcmp(loadedTexture.path.data(), path.C_Str()) == 0)
            {
                textures.push_back(loadedTexture);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture{Texture::loadFromFile(params.directory + '/' + path.C_Str())};
            texture.type = typeName;
            texture.path = path.C_Str();
            textures.push_back(texture);
            params.textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}
