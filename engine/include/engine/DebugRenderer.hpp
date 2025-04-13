#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "Clock.hpp"
#include "Geometry.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Singleton.hpp"
#include "Transform.hpp"
#include <glm/glm.hpp>
#include <memory>

class DebugRenderer : public Singleton<DebugRenderer> {
public:
  using Color = glm::vec3;
  static constexpr Color defaultColor = Util::green;

  void startup() {
    sphere =
        std::make_shared<Mesh>(std::move(Geometry::makeSphere(1000).load()));
    coordinatesystem = std::make_shared<Mesh>(
        std::move(Geometry::makeCoordinateMesh().load()));
    ray = std::make_shared<Mesh>(
        std::move(Geometry::makeLine(glm::vec3{}, glm::vec3{0, 0, -1}).load()));
  }

  void shutdown() {
    shapes.clear();
    sphere = nullptr;
    coordinatesystem = nullptr;
    ray = nullptr;
  }

  void addRay(glm::vec3 from, glm::vec3 to, const Color color = defaultColor,
              Duration lifetime = Duration{0}) {
    const auto a = glm::normalize(to - from);
    const auto b = glm::vec3{0, 0, -1};
    const auto cross = glm::cross(a, b);
    const float dot = glm::dot(a, b);

    Transform t;
    t.position = from;
    t.scale = glm::vec3{1.0f} * glm::distance(from, to);

    if (glm::length(cross) < std::numeric_limits<float>::epsilon() &&
        dot <= 0) {
      t.rotation = glm::angleAxis(glm::pi<float>(), glm::vec3{1, 0, 0});
    } else {
      t.rotation = glm::angleAxis(-std::atan2(glm::length(cross), dot),
                                  glm::normalize(cross));
    }

    shapes.emplace_back(t, ray, color, lifetime);
  }

  void addSphere(const glm::vec3 &pos, float r,
                 const Color &color = defaultColor,
                 Duration lifetime = Duration{0}) {
    Transform t;
    t.position = pos;
    t.scale *= r;
    shapes.emplace_back(t, sphere, color, lifetime);
  }

  void addSphere(Transform transform, float r, const Color &color,
                 Duration lifetime = Duration{0}) {
    transform.scale = r * glm::vec3{1.0f, 1.0f, 1.0f};
    shapes.emplace_back(transform, sphere, color, lifetime);
  }

  void update(Duration dt) {
    shapes.erase(std::remove_if(shapes.begin(), shapes.end(),
                                [](const auto &shape) {
                                  return shape.lifetime < Duration{0};
                                }),
                 shapes.end());

    for (auto &shape : shapes) {
      shape.lifetime -= dt;
    }
  }

  void render() {
    ShaderHandle handle(Renderer::ShaderID::Color);

    handle.shader().setMat4("projection", gRenderer.getProjection());
    handle.shader().setMat4("view", gRenderer.getView());

    for (const auto &shape : shapes) {
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
  std::shared_ptr<Mesh> ray;
};

static DebugRenderer &gDebugRenderer = DebugRenderer::instance();

#endif
