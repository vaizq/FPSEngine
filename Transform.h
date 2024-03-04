//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_TRANSFORM_H
#define FPSFROMSCRATCH_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
    glm::vec3 position;
    float yaw{};
    float pitch{};
    float scale{1.0f};

    glm::mat4 modelMatrix() const
    {
        auto m = glm::translate(glm::mat4{1.0f}, position);
        m = glm::rotate(m, yaw, glm::vec3(0.f, 1.f, 0.f));
        m = glm::rotate(m, pitch, glm::vec3(1.f, 0.f, 0.f));
        m = glm::scale(m, glm::vec3{scale});
        return m;
    }
};


#endif //FPSFROMSCRATCH_TRANSFORM_H
