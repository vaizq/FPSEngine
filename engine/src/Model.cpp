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
#include <limits>
#include "engine/Util.hpp"


glm::mat4 aiToGlmMat(const aiMatrix4x4& from) {
    glm::mat4 to;


    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

void printSkeleton(const std::vector<Joint>& skeleton, size_t index = 0, int numParents = 0) {
    std::string msg;
    for (int i = 0; i < numParents; i++) {
        msg += " ";
    }
    msg += skeleton[index].name;

    printf("%s\n", msg.c_str());

    for (int i = 0; i < skeleton.size(); i++) {
        if (skeleton[i].parent == index) {
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
    skeleton = AssetImporter::loadSkeleton(scene);
    processNode(scene->mRootNode, scene);
    animations = AssetImporter::loadAnimations(scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
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
        const unsigned boneId = skeleton.boneIDs[bone->mName.C_Str()];

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
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return {vertices, indices, textures};
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(auto& loadedTexture : textures_loaded)
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
            Texture texture{Texture::loadFromFile(this->directory + '/' + path.C_Str())};
            texture.type = typeName;
            texture.path = path.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}
