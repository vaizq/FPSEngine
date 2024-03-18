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


struct Perspective
{
    float fow{45.f};
    float width{666.f};
    float height{420.f};
    float near{0.1f};
    float far{100.f};
};

// Easy drawing of primitives for development
// Centralizing rendering
// Separating rendering from game logic
class Renderer : public Singleton<Renderer>
{
    friend class Singleton<Renderer>;
public:
    using Color = glm::vec3;
    static unsigned int drawMode;
    Perspective activePerspective{};
    Camera activeCamera;
    Shader& activeShader;
    void drawRay(const RayCast& ray, const Color& color = Util::red);
    void drawSphere(const glm::vec3& position, float radius = 1.0f, const Color& color = Util::red);
private:
    Renderer();
    Mesh mSphereMesh;
};


#endif //FPSFROMSCRATCH_RENDERER_H
