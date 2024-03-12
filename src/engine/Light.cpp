//
// Created by vaige on 12.3.2024.
//

#include "Light.h"
#include "ResourceManager.h"
#include "Geometry.h"

Light::Light()
:   GameObject(),
    mMesh{Geometry::makeBox()}
{}

void Light::render(Shader& shader, const glm::mat4& parentTransform)
{
    auto& colorShader = ResourceManager::instance().getShader("color");
    colorShader.use();
    colorShader.setMat4("view", ResourceManager::instance().view);
    colorShader.setMat4("projection", ResourceManager::instance().projection);
    colorShader.setMat4("model", parentTransform * transform.modelMatrix());
    colorShader.setVec3("color", glm::vec3{1.0f, 1.0f, 1.0f});
    mMesh.draw(colorShader);
    shader.use();
}