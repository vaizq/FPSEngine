//
// Created by vaige on 13.3.2024.
//

#include "Renderer.h"
#include "Geometry.h"
#include "ResourceManager.h"
#include "ResourceManager.h"


void Renderer::drawRay(const RayCast& ray, const Color& color)
{

}

void Renderer::drawSphere(const glm::vec3& position, float radius, const Color& color)
{
    glm::mat4 view = activeCamera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
            glm::radians(activePerspective.fow),
            (float)activePerspective.width / (float)activePerspective.height,
            activePerspective.near,
            activePerspective.far);

    auto mm = glm::translate(glm::mat4{1.0f}, position);
    mm = glm::scale(mm, glm::vec3{radius});

    activeShader.use();
    activeShader.setMat4("view", view);
    activeShader.setMat4("projection", projection);
    activeShader.setMat4("model", mm);
    activeShader.setMat4("normalMatrix", glm::transpose(glm::inverse(mm)));
    activeShader.setVec3("color", color);
    mSphereMesh.draw(activeShader, drawMode);
}

GLenum Renderer::drawMode{GL_TRIANGLES};

Renderer::Renderer()
:
    activeShader{ResourceManager::instance().getShader("color")},
    mSphereMesh{Geometry::makeSphere()}
{}
