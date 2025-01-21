#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "Clock.hpp"
#include <glm/glm.hpp>
#include <memory>
#include "ResourceManager.hpp"
#include "Singleton.hpp"
#include "Geometry.hpp"
#include "Transform.hpp"
#include "Renderer.hpp"


class DebugRenderer : public Singleton<DebugRenderer> {
public:
    using Color = glm::vec3;

    void startup() {
        sphere = std::make_shared<Mesh>(Geometry::makeSphere(1000));
        coordinatesystem = std::make_shared<Mesh>(Geometry::makeCoordinateMesh());
    }

    void shutdown() {
        shapes.clear();
        sphere = nullptr;
        coordinatesystem = nullptr;
    }

    void addSphere(const glm::vec3& pos, float r, const Color& color, Duration lifetime = Duration{0}) {
        Transform t;
        t.position = pos;
        t.scale *= r;
        shapes.emplace_back(t, sphere, color, lifetime);
    }

    void addSphere(Transform transform, float r, const Color& color, Duration lifetime = Duration{0}) {
        transform.scale = r * glm::vec3{1.0f, 1.0f, 1.0f};
        shapes.emplace_back(transform, sphere, color, lifetime);
    }

    void update(Duration dt) {
        shapes.erase(std::remove_if(shapes.begin(), shapes.end(), [](const auto& shape) {
            return shape.lifetime < Duration{0};
        }), shapes.end());

        for (auto& shape : shapes) {
            shape.lifetime -= dt;
        }
    }

    void render() {
        ShaderHandle handle(Renderer::ShaderID::Color);

        handle.shader().setMat4("projection", gRenderer.getProjection());
        handle.shader().setMat4("view", gRenderer.getView());

        for (const auto& shape : shapes) {
            const auto modelMatrix = shape.transform.modelMatrix();
            const auto normalMatrix = glm::transpose(glm::inverse(modelMatrix));
            handle.shader().setMat4("model", modelMatrix);
            handle.shader().setMat4("normalMatrix", normalMatrix);
            handle.shader().setVec3("color", shape.color);
            shape.mesh->draw(handle.shader(), GL_LINES);
        }
    }

private:
    struct Shape {
        Transform transform;
        std::shared_ptr<Mesh> mesh;
        Color color;
        Duration lifetime;
    };

    std::vector<Shape> shapes;
    std::shared_ptr<Mesh> sphere;
    std::shared_ptr<Mesh> coordinatesystem;
};

static DebugRenderer& gDebugRenderer = DebugRenderer::instance();


#endif
