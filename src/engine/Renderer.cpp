//
// Created by vaige on 13.3.2024.
//
#include "Renderer.h"
#include "ResourceManager.h"

GLenum Renderer::drawMode{GL_TRIANGLES};


DebugRenderer &DebugRenderer::instance()
{
    static DebugRenderer self;
    return self;
}

void DebugRenderer::drawPoint(const glm::vec3 &pos, const glm::vec3 &color)
{
    mPoints.push_back(pos);
}

void DebugRenderer::drawLine(const glm::vec3 &point, const glm::vec3 &direction, const glm::vec3 &color)
{
    mLines.push_back(Line{point, direction});
}

void DebugRenderer::drawVector(const glm::vec3 &pos, const glm::vec3 &vector, const glm::vec3 &color)
{

}

void DebugRenderer::drawTransform(const Transform &transform)
{

}

void DebugRenderer::render()
{
    auto pointMesh = [this]() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned int index{0};
        for (const auto& point : mPoints) {
            vertices.push_back(Vertex{point});
            indices.push_back(index++);
        }
        return Mesh{std::move(vertices), std::move(indices), {}};
    }();

    auto lineMesh = [this]() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned int index{0};
        for (const auto& line : mLines) {
            Vertex a{line.point - line.direction * 100000.0f};
            Vertex b{line.point + line.direction * 100000.0f};
            vertices.push_back(a);
            vertices.push_back(b);
            indices.push_back(index++);
            indices.push_back(index++);
        }
        return Mesh{std::move(vertices), std::move(indices), {}};
    }();

    auto shader = ResourceManager::instance().useShader("color");
    shader.setVec3("color", Util::red);
    pointMesh.draw(shader, GL_POINT);
    lineMesh.draw(shader, GL_LINE);

    mPoints.clear();
    mLines.clear();
}