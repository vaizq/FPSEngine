//
// Created by vaige on 19.2.2024.
//

#include "Mesh.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, textureCoordinate));
    glEnableVertexAttribArray(2);
}


/*
 * uniform TextureID diffuse0;
 * uniform TextureID diffuse1;
 * uniform TextureID specular0;
 */
void Mesh::draw(Shader &shader)
{
    shader.use();

    int diffuseID{0};
    int specularID{0};

    for (int i = 0; i < mTextures.size(); i++) {

        glActiveTexture(GL_TEXTURE0 + i);

        const Texture& tex = mTextures[i];
        switch (tex.type) {
            case Texture::Type::Diffuse:
                shader.setUInt("Diffuse" + std::to_string(diffuseID++), i);
                break;
            case Texture::Type::Specular:
                shader.setUInt("Specular" + std::to_string(specularID++), i);
                break;
        }

        glBindTexture(GL_TEXTURE_2D, tex.id);
    }

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
}

Texture Texture::loadTexture(std::string path, Texture::Type type)
{
    Texture tex;
    tex.type = type;
    int x, y, n;
    unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 0);

    if (data) {
        GLenum format = [n](){
            switch (n) {
                case 1:
                    return GL_RED;
                case 3:
                    return GL_RGB;
                case 4:
                    return GL_RGBA;
                default:
                    return GL_RGB;
            }
        }();

        glGenTextures(1, &tex.id);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        stbi_image_free(data);
        throw std::runtime_error("Unable to load texture from " + path);
    }

    tex.path = std::move(path);
    return tex;
}





















