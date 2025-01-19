#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "Clock.hpp"
#include <glm/glm.hpp>
#include "ResourceManager.hpp"
#include "Singleton.hpp"
#include "Geometry.hpp"
#include "Transform.hpp"
#include "Renderer.hpp"


class DebugRenderer : public Singleton<DebugRenderer> {
public:
    using Color = glm::vec3;

    void startup() {
        sphere = std::make_unique<Mesh>(Geometry::makeSphere(1));
    }

    void shutdown() {
        sphere = nullptr;
    }

    void addSphere(const glm::vec3& pos, float r, const Color& color, Duration lifetime = Duration::max()) {
        Transform t;
        t.position = pos;
        t.scale *= r;
        shapes.emplace_back(t, color, lifetime);
    }

    void addSphere(Transform transform, float r, const Color& color, Duration lifetime = Duration::max()) {
        transform.scale = r * glm::vec3{1.0f, 1.0f, 1.0f};
        shapes.emplace_back(transform, color, lifetime);
    }

    void update(Duration dt) {
        for (auto& shape : shapes) {
            shape.lifetime -= dt;
        }

        shapes.erase(std::remove_if(shapes.begin(), shapes.end(), [](const auto& shape) {
            return shape.lifetime <= Duration{0};
        }), shapes.end());
    }

    void render() {
        auto& shader = gRenderer.pushShader(Renderer::ShaderID::Color);

        shader.setMat4("projection", gRenderer.getProjection());
        shader.setMat4("view", gRenderer.getView());

        for (const auto& shape : shapes) {
            const auto modelMatrix = shape.transform.modelMatrix();
            const auto normalMatrix = glm::transpose(glm::inverse(modelMatrix));
            shader.setMat4("model", modelMatrix);
            shader.setMat4("normalMatrix", normalMatrix);
            shader.setVec3("color", shape.color);
        }

        gRenderer.popShader();
    }

private:
    struct Sphere {
        Transform transform;
        Color color;
        Duration lifetime;
    };

    std::vector<Sphere> shapes;
    std::unique_ptr<Mesh> sphere;
};

static DebugRenderer& gDebugRenderer = DebugRenderer::instance();


#endif
