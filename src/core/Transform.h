//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_TRANSFORM_H
#define FPSFROMSCRATCH_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <nlohmann/json_fwd.hpp>


static constexpr glm::vec3 worldUp{0.0f, 1.0f, 0.0f};
static constexpr float yawFix{glm::radians(-90.f)};

struct Transform
{
    glm::vec3 position{};
    float yaw{};
    float pitch{};
    float scale{1.0f};

    Transform() = default;

    Transform(const glm::vec3& position, float yaw, float pitch, float scale)
    : position{position}, yaw{yaw}, pitch{pitch}, scale{scale}
    {}

    explicit Transform(const glm::mat4& matrix)
    : position{matrix[3]}
    {
        glm::vec3 rotation = glm::eulerAngles(glm::quat_cast(matrix));
        yaw = rotation.y;
        pitch = rotation.x;
        scale = glm::length(glm::vec3(matrix[0]));
    }

    [[nodiscard]] glm::mat4 modelMatrix() const
    {
        auto m = glm::translate(glm::mat4{1.0f}, position);
        m = glm::rotate(m, -yaw, glm::vec3(0.f, 1.f, 0.f));
        m = glm::rotate(m, pitch, glm::vec3(1.f, 0.f, 0.f));
        m = glm::scale(m, glm::vec3{scale});
        return m;
    }

    [[nodiscard]] glm::vec3 front() const
    {
        return glm::normalize(glm::vec3(std::cos(yaw + yawFix) * std::cos(pitch), std::sin(pitch), std::sin(yaw + yawFix) * std::cos(pitch)));
    }

    [[nodiscard]] glm::vec3 right() const
    {
        return glm::normalize(glm::cross(front(), worldUp));
    }

    [[nodiscard]] glm::vec3 up() const
    {
        return glm::normalize(glm::cross(right(), front()));
    }

    static nlohmann::json serialize(const Transform& transform);
    static Transform deserialize(const nlohmann::json& j);
};

static Transform operator+(const Transform& lhs, const Transform& rhs)
{
    return Transform{lhs.position + rhs.position, lhs.yaw + rhs.yaw, lhs.pitch + rhs.pitch, lhs.scale + rhs.scale};
}



#endif //FPSFROMSCRATCH_TRANSFORM_H
