//
// Created by vaige on 12.3.2024.
//

#include "Light.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"
#include "Geometry.hpp"

Light::Light()
:   GameObject(),
    mMesh{Geometry::makeBox()}
{}

void Light::render(Shader& shader, const glm::mat4& parentTransform)
{
    /*
    auto& colorShader = gRenderer.pushShader(Renderer::ShaderID::Color);

    colorShader.use();
    colorShader.setMat4("view", gRenderer.getView());
    colorShader.setMat4("projection", gRenderer.getProjection());
    colorShader.setMat4("model", parentTransform * transform.modelMatrix());
    colorShader.setVec3("color", glm::vec3{1.0f, 1.0f, 1.0f});
    mMesh.draw(colorShader);

    gRenderer.popShader();
    */
}
