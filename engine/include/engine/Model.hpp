//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_MODEL_H
#define FPSFROMSCRATCH_MODEL_H

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

using namespace std;


class Model
{
public:
    // model data
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
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
        for(auto& mesh : meshes)
            mesh.draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene, const aiMatrix4x4& parentTransform);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene, const aiMatrix4x4& transform);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};


#endif //FPSFROMSCRATCH_MODEL_H
