//
// Created by vaige on 19.2.2024.
//

#include "Mesh.h"
#include "glad/glad.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
: mVertices(std::move(vertices)), mIndices(std::move(indices)), mTextures(std::move(textures))
{
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

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);
}

Mesh::~Mesh()
{
    deleteBuffers();
}


/*
 * uniform sampler2D diffuse0;
 * uniform sampler2D diffuse1;
 * uniform sampler2D specular0;
 */
void Mesh::draw(Shader &shader, GLenum mode)
{
    shader.use();

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
    glDrawElements(mode, mIndices.size(), GL_UNSIGNED_INT, 0);
}

Mesh::Mesh(Mesh &&other) noexcept
{
    *this = std::move(other);
}

Mesh &Mesh::operator=(Mesh &&other) noexcept
{
    if (this != &other) {
        deleteBuffers();
        mVertices = std::move(other.mVertices);
        mIndices = std::move(other.mIndices);
        mTextures = std::move(other.mTextures);
        mVAO = other.mVAO;
        mVBO = other.mVBO;
        mEBO = other.mEBO;

        other.mVAO = 0;
        other.mVBO = 0;
        other.mEBO = 0;
    }

    return *this;
}

void Mesh::deleteBuffers()
{
    glDeleteBuffers(1, &mEBO);
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}