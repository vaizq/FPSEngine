//
// Created by vaige on 19.2.2024.
//

#include "Mesh.hpp"
#include "DebugRenderer.hpp"
#include "glad/glad.h"
#include "engine/ResourceManager.hpp"
#include "engine/DebugRenderer.hpp"
#include "Util.hpp"
#include <limits>


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
:   mVertices{std::move(vertices)}, 
    mIndices{std::move(indices)}, 
    mTextures{std::move(textures)}, 
    mNumIndices{mIndices.size()}
{}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &mEBO);
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

Mesh::Mesh(Mesh &&other) noexcept
: mNumIndices{other.mNumIndices}
{
    *this = std::move(other);
}

Mesh& Mesh::operator=(Mesh &&other) noexcept
{
    if (this != &other) {
        mVertices = std::move(other.mVertices);
        mIndices = std::move(other.mIndices);
        mTextures = std::move(other.mTextures);
        mNumIndices = other.mNumIndices;

        mVAO = other.mVAO;
        mVBO = other.mVBO;
        mEBO = other.mEBO;

        other.mVAO = 0;
        other.mVBO = 0;
        other.mEBO = 0;
    }

    return *this;
}

void Mesh::load() {
    for (auto& texture : mTextures) {
        texture.load();
    }

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // textureCoordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    // tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // boneids
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(5);

    // boneweights
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, boneWeights));
    glEnableVertexAttribArray(6);

    // Save some ram
    mVertices.clear();
    mIndices.clear();
}

void Mesh::draw(Shader &shader, GLenum mode)
{
    int diffuseID{0};
    int specularID{0};

    for (int i = 0; i < mTextures.size(); i++) {

        glActiveTexture(GL_TEXTURE0 + i);

        const Texture& tex = mTextures[i];
        if (tex.type == "texture_diffuse") {
            shader.setInt("texture_diffuse" + std::to_string(diffuseID++), i);
        }
        else if (tex.type == "texture_specular") {
            shader.setInt("texture_specular" + std::to_string(specularID++), i);
        }

        glBindTexture(GL_TEXTURE_2D, tex.id);
    }

    glBindVertexArray(mVAO);
    glDrawElements(mode, mNumIndices, GL_UNSIGNED_INT, 0);
}
