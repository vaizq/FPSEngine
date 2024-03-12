//
// Created by vaige on 6.3.2024.
//

#ifndef FPSFROMSCRATCH_PARTICLEEMITTER_H
#define FPSFROMSCRATCH_PARTICLEEMITTER_H


#include "GameObject.h"
#include "ResourceManager.h"
#include "Geometry.h"
#include "Util.h"
#include "Random.h"
#include <deque>
#include <chrono>


struct Particle
{
    Mesh& mesh;
    Transform transform;
    glm::vec3 velocity;
    std::chrono::time_point<std::chrono::steady_clock> creationTime;
};

//constexpr glm::vec3 gravity{0.0f, -20.0f, 0.0f};


class ParticleEmitter : public GameObject
{
public:
    ParticleEmitter()
    : mBloodUnitMesh{Geometry::makeBox()}
    {}


    virtual void onGUI() override
    {
        GameObject::onGUI();
        ImGui::DragFloat("rate", &rate);
        ImGui::DragFloat("velocity", &velocity);
    }

    virtual void update(Duration dt) override
    {
        for (auto& particle : mParticles) {
            particle.velocity += 0.5f * gravity * dt.count();
            particle.transform.position += particle.velocity * dt.count();
        }

        while (std::chrono::steady_clock::now() - mParticles.front().creationTime > 2 * std::chrono::seconds{1}) {
            mParticles.pop_front();
        }

        if (mFromPrevParticle > (Duration{1.f} / rate)) {
            Particle p{.mesh = mBloodUnitMesh};
            p.transform = transform;
            p.velocity = createVelocity();
            p.creationTime = std::chrono::steady_clock::now();
            mParticles.push_back(p);
            mFromPrevParticle = Duration{};
        }

        mFromPrevParticle += dt;
    }

    void render(Shader& unusedShader)
    {
        Shader& shader = ResourceManager::instance().getShader("color");
        shader.use();
        shader.setMat4("view", ResourceManager::instance().view);
        shader.setMat4("projection", ResourceManager::instance().projection);

        const auto modelMatrix = worldTransform().modelMatrix();
        shader.setVec3("color", Util::red);
        for (auto& particle : mParticles) {
            shader.setMat4("model", modelMatrix * particle.transform.modelMatrix());
            particle.mesh.draw(shader);
        }

        unusedShader.use();
    }

    float rate{1.f};
    glm::vec3 direction{worldUp};
    float velocity{10.f};
    float angle{glm::radians(45.f)};
private:
    glm::vec3 createVelocity()
    {
        // Generate non zero random float in range [0, 1]
        auto rf = []() {
            float x = randomFloat();
            while (x == 0.0f) {
                x = randomFloat();
            }
            return x;
        };

        return velocity * glm::normalize(glm::vec3{rf(), rf(), rf()});
    }

    Mesh mBloodUnitMesh;
    std::deque<Particle> mParticles;
    Duration mFromPrevParticle{};
};

#endif //FPSFROMSCRATCH_PARTICLEEMITTER_H


