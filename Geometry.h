//
// Created by vaige on 24.2.2024.
//

#ifndef FPSFROMSCRATCH_GEOMETRY_H
#define FPSFROMSCRATCH_GEOMETRY_H

#include "Mesh.h"
#include <glm/glm.hpp>

namespace Geomery
{
    static Mesh createSphere()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        constexpr float R = 1.0f;
        constexpr float delta = R / 100;
        constexpr float deltaAngle = glm::radians(360.0f) / 100;

        unsigned int i = 0;
        for (float y = -R; y <= R; y += delta) {
            const float r = std::sqrt(R * R - y * y);
            for (float a = 0.0f; a < glm::radians(360.0f); a += deltaAngle) {
                float x = r * std::cos(a);
                float z = r * std::sin(a);
                vertices.push_back(Vertex{{x, y, z},
                                          {},
                                          {}});
                indices.push_back(i++);
            }
        }

        return Mesh{vertices, indices, {}};
    }

    static Mesh createSphere2(int nVertices)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        constexpr float R = 1.0f;
        const int nHeight = (int)std::sqrt((float) nVertices);
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

                vertices.push_back(Vertex{{x, y, z},{},{}});
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
}

#endif //FPSFROMSCRATCH_GEOMETRY_H
