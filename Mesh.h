//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_MESH_H
#define FPSFROMSCRATCH_MESH_H

#include "Shader.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "Texture.h"


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinate;
};



class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures);
    ~Mesh();
    void draw(Shader& shader, GLenum mode = GL_TRIANGLES);
private:
    std::vector<Vertex> mVertices;
    std::vector<unsigned> mIndices;
    std::vector<Texture> mTextures;
    unsigned mVAO{};
    unsigned mVBO{};
    unsigned mEBO{};
};


#endif //FPSFROMSCRATCH_MESH_H

