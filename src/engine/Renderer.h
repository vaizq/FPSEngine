//
// Created by vaige on 13.3.2024.
//

#ifndef FPSFROMSCRATCH_RENDERER_H
#define FPSFROMSCRATCH_RENDERER_H

#include "Camera.h"
#include "Singleton.h"
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "RayCast.h"
#include "Util.h"


class Renderer
{
public:
    static GLenum drawMode;
};


// Draw primitives in world space for debugging and prototyping
class DebugRenderer
{
public:
    DebugRenderer(const DebugRenderer&) = delete;
    DebugRenderer(DebugRenderer&&) = delete;
    DebugRenderer& operator=(const DebugRenderer&) = delete;
    DebugRenderer& operator=(DebugRenderer&&) = delete;

    static DebugRenderer& instance();

    static constexpr auto defColor = Util::blue;

    void drawPoint(const glm::vec3& pos, const glm::vec3& color = defColor);
    void drawLine(const glm::vec3& point, const glm::vec3& direction, const glm::vec3& color = defColor);
    void drawVector(const glm::vec3& pos, const glm::vec3& vector, const glm::vec3& color = defColor);
    void drawTransform(const Transform& transform);

    void render();

private:
    DebugRenderer() = default;

    struct Line {
        glm::vec3 point;
        glm::vec3 direction;
    };

    struct Vector {
        glm::vec3 tail;
        glm::vec3 vec;
    };

    std::vector<glm::vec3> mPoints;
    std::vector<Line> mLines;
    std::vector<Vector> mVectors;
    std::vector<Transform> mTransforms;
};


#endif //FPSFROMSCRATCH_RENDERER_H
