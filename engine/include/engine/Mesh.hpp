//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_MESH_H
#define FPSFROMSCRATCH_MESH_H

#include "Shader.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "Texture.hpp"

struct Vertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texCoord{};
    glm::vec3 tangent{};
    glm::vec3 bitangent{};
    glm::ivec4 boneIDs{0};
    glm::vec4 boneWeights{0.0f};
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures);
    ~Mesh();

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh& load();

    void draw(Shader& shader, GLenum mode = GL_TRIANGLES);

private:
    std::vector<Vertex> mVertices;
    std::vector<unsigned> mIndices;
    std::vector<Texture> mTextures;

    size_t mNumIndices;
    unsigned mVAO{};
    unsigned mVBO{};
    unsigned mEBO{};
};

#endif //FPSFROMSCRATCH_MESH_H
