//
// Created by vaige on 24.2.2024.
//

#ifndef FPSFROMSCRATCH_GEOMETRY_H
#define FPSFROMSCRATCH_GEOMETRY_H

#include "Mesh.h"
#include <glm/glm.hpp>
#include "Perlin.h"

namespace Geometry
{
    Mesh makeSphere(int nVertices = 10)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        constexpr float R = 1.0f;
        const int nHeight = (int) std::sqrt((float) nVertices);
        const int nRim = nHeight;

        const float delta = 2 * R / nHeight;
        const float deltaAngle = glm::radians(360.0f) / nRim;

        for (int yi = 0; yi <= nHeight; ++yi) {
            const float y = yi * delta - R;
            const float r = std::sqrt(R * R - y * y);
            for (int ai = 0; ai < nRim; ++ai) {
                float angle = ai * deltaAngle;
                float x = r * std::cos(angle);
                float z = r * std::sin(angle);
                glm::vec3 pos{x, y, z};
                vertices.emplace_back(pos, glm::normalize(pos), glm::vec2{});
            }
        }

        // Create first indices
        for (int i = 1; i < nRim; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        {
            indices.push_back(0);
            indices.push_back(nRim);
            indices.push_back(1);
        }

        // Create rectangles
        const int topIndex = vertices.size() - 1;
        for (int i = 1; i < topIndex; ++i) {
            int upper = i + nRim;
            int right = i + 1;
            int rightUpper = upper + 1;

            indices.push_back(i);
            indices.push_back(upper);
            indices.push_back(right);

            indices.push_back(right);
            indices.push_back(upper);
            indices.push_back(rightUpper);
        }

        // Connect upmost rim to the top point
        const int first = topIndex - nRim;
        for (int i = first; i < topIndex - 1; ++i) {
            indices.push_back(i);
            indices.push_back(topIndex);
            indices.push_back(i + 1);
        }
        {
            indices.push_back(topIndex - 1);
            indices.push_back(topIndex);
            indices.push_back(first);
        }

        return Mesh{vertices, indices, {}};
    }

    Mesh makeBox(const glm::vec3 &size = glm::vec3{1.0f, 1.0f, 1.0f})
    {
        const float dx = size.x / 2.0f;
        const float dy = size.y / 2.0f;
        const float dz = size.z / 2.0f;

        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;


        auto makeVertex = [&vertices](const glm::vec3 &pos)
        {
            vertices.emplace_back(pos, glm::normalize(pos), glm::vec2{});
        };

        /*
                    6----------7
                   /|         /|
                  / |        / |
                 2----------3  |
                 |  |       |  |
                 |  |       |  |
                 |  4-------|--5
                 | /        | /
                 |/         |/
                 0----------1
        */

        makeVertex({-dx, -dy, dz});
        makeVertex({dx, -dy, dz});
        makeVertex({-dx, dy, dz});
        makeVertex({dx, dy, dz});
        makeVertex({-dx, -dy, -dz});
        makeVertex({dx, -dy, -dz});
        makeVertex({-dx, dy, -dz});
        makeVertex({dx, dy, -dz});

        // front
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(3);

        // right
        indices.push_back(5);
        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(5);
        indices.push_back(3);
        indices.push_back(7);

        // top
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(6);
        indices.push_back(3);
        indices.push_back(6);
        indices.push_back(7);

        // bottom
        indices.push_back(5);
        indices.push_back(4);
        indices.push_back(0);
        indices.push_back(5);
        indices.push_back(0);
        indices.push_back(1);

        // left
        indices.push_back(0);
        indices.push_back(4);
        indices.push_back(6);
        indices.push_back(0);
        indices.push_back(6);
        indices.push_back(2);

        // back
        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(7);
        indices.push_back(4);
        indices.push_back(7);
        indices.push_back(6);


        return Mesh{std::move(vertices), std::move(indices), {}};
    }

    Mesh makeBoxWireframe(const glm::vec3 &size = glm::vec3{1.0f, 1.0f, 1.0f})
    {
        const float dx = size.x / 2.0f;
        const float dy = size.y / 2.0f;
        const float dz = size.z / 2.0f;

        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        auto makeVertex = [&vertices](const glm::vec3 &pos)
        {
            vertices.emplace_back(pos, glm::normalize(pos), glm::vec2{});
        };

        /*
                    6----------7
                   /|         /|
                  / |        / |
                 2----------3  |
                 |  |       |  |
                 |  |       |  |
                 |  4-------|--5
                 | /        | /
                 |/         |/
                 0----------1
        */

        makeVertex({-dx, -dy, dz});
        makeVertex({dx, -dy, dz});
        makeVertex({-dx, dy, dz});
        makeVertex({dx, dy, dz});
        makeVertex({-dx, -dy, -dz});
        makeVertex({dx, -dy, -dz});
        makeVertex({-dx, dy, -dz});
        makeVertex({dx, dy, -dz});

        // front
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(0);

        // right
        indices.push_back(1);
        indices.push_back(5);
        indices.push_back(1);
        indices.push_back(3);
        indices.push_back(3);
        indices.push_back(7);
        indices.push_back(5);
        indices.push_back(7);

        // bottom
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(1);
        indices.push_back(5);
        indices.push_back(5);
        indices.push_back(4);
        indices.push_back(0);
        indices.push_back(4);

        // left
        indices.push_back(0);
        indices.push_back(4);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(6);
        indices.push_back(4);
        indices.push_back(6);

        // back
        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(5);
        indices.push_back(7);
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(4);
        indices.push_back(6);


        return Mesh{std::move(vertices), std::move(indices), {}};
    }

    Mesh makePlane(float width, float height, std::vector<Texture> textures = {})
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        constexpr glm::vec3 up{0.0f, 1.0f, 0.0f};
        const float dx = width / 2.0f;
        const float dz = height / 2.0f;


        vertices.emplace_back(glm::vec3{-dx, 0.f, dz},
                              glm::normalize(up),
                              glm::vec2{0.0f, 0.0f});

        vertices.emplace_back(glm::vec3{-dx, 0.f, -dz},
                              glm::normalize(up),
                              glm::vec2{0.0f, 1.0f});

        vertices.emplace_back(glm::vec3{dx, 0.f, -dz},
                              glm::normalize(up),
                              glm::vec2{1.0f, 1.0f});

        vertices.emplace_back(glm::vec3{dx, 0.f, dz},
                              glm::normalize(up),
                              glm::vec2{1.0f, 0.0f});

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);

        return Mesh{std::move(vertices), std::move(indices), std::move(textures)};
    }

    Mesh makeCoordinateMesh()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        vertices.emplace_back(glm::vec3{-1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, -1.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, 1.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, 0.f, -1.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, 0.f, 1.f}, glm::vec3{}, glm::vec2{});

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(4);
        indices.push_back(5);

        return Mesh{std::move(vertices), std::move(indices), {}};
    }

    Mesh makeXAxis()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        vertices.emplace_back(glm::vec3{-1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});

        indices.push_back(0);
        indices.push_back(1);

        return Mesh{std::move(vertices), std::move(indices), {}};
    }

    Mesh makeYAxis()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        vertices.emplace_back(glm::vec3{0.f, -1.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, 1.f, 0.f}, glm::vec3{}, glm::vec2{});

        indices.push_back(0);
        indices.push_back(1);

        return Mesh{std::move(vertices), std::move(indices), {}};
    }

    Mesh makeZAxis()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        vertices.emplace_back(glm::vec3{0.f, 0.f, -1.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, 0.f, 1.f}, glm::vec3{}, glm::vec2{});

        indices.push_back(0);
        indices.push_back(1);

        return Mesh{std::move(vertices), std::move(indices), {}};
    }

    Mesh makeCrosshire()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        vertices.emplace_back(glm::vec3{-1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, -1.f, 0.f}, glm::vec3{}, glm::vec2{});
        vertices.emplace_back(glm::vec3{0.f, 1.f, 0.f}, glm::vec3{}, glm::vec2{});

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(3);

        return Mesh{std::move(vertices), std::move(indices), {}};
    }

    Mesh makePerlinTerrain(int width, int height, float scale = 1.0f, std::vector<Texture> textures = {})
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned> indices;

        Perlin2D perlin(width, height);

        // Calculate vertices
        for (int zi = 0; zi < height; ++zi) {
            for (int xi = 0; xi < width; ++xi) {
                auto x = static_cast<float>(xi);
                auto z = static_cast<float>(zi);
                float y = perlin(glm::vec2(x + 0.5f, z + 0.5f)) * scale;
                vertices.emplace_back(glm::vec3(x, y, z), glm::vec3{}, glm::vec2{});
            }
        }

        auto vertexIndex= [width, height](int xi, int yi) {
            return (yi * width) + xi;
        };

        // Each quad has an index
        // Each vertex has an index
        // Each vertex has a xi and yi

        for (int qi = 0; qi < width * height; ++qi) {
            int zi = qi / width;
            int xi = qi % width;
            int ai = zi * (width + 1) + xi;
            int bi = ai + width + 1;
            int ci = bi + 1;
            int di = ai + 1;

            auto n0 = glm::normalize(glm::cross(vertices[di].position - vertices[ai].position, vertices[bi].position - vertices[ai].position));
            auto n1 = glm::normalize(glm::cross(vertices[bi].position - vertices[ci].position, vertices[di].position - vertices[ci].position));
            vertices[ai].normal = n0;
            vertices[bi].normal = n0;
            vertices[di].normal = n0;
            vertices[ci].normal = n1;

            indices.push_back(ai);
            indices.push_back(bi);
            indices.push_back(di);

            indices.push_back(di);
            indices.push_back(bi);
            indices.push_back(ci);
        }

        return Mesh{std::move(vertices), std::move(indices), std::move(textures)};
    }
}

#endif //FPSFROMSCRATCH_GEOMETRY_H







