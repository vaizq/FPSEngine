#ifndef WEAPON_HPP
#define WEAPON_HPP


#include "engine/GameObject.hpp"
#include "engine/DebugRenderer.hpp"


class Weapon : public GameObject {
public:
    void render(Shader& shader, const glm::mat4& parentTransform) override {
        Transform t{parentTransform * transform.modelMatrix()};
        gDebugRenderer.addRay(t.position, t.position + 100.0f * t.front());

        for (auto& child : children) {
            child->render(shader, t.modelMatrix());
        }
    }

private:
};


#endif
