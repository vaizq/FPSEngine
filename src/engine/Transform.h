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


// Transform defines a coordinate system
struct Transform
{
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{1.0f};

    Transform() = default;

    Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    : position{position}, rotation{rotation}, scale{scale}
    {}

    explicit Transform(const glm::mat4& matrix)
    : position{matrix[3]}
    {
        rotation = glm::eulerAngles(glm::quat_cast(matrix));
        scale.x = glm::length(glm::vec3(matrix[0]));
        scale.y = glm::length(glm::vec3(matrix[1]));
        scale.z = glm::length(glm::vec3(matrix[2]));
    }

    [[nodiscard]] glm::mat4 rotationMatrix() const
    {
        return glm::mat4_cast(glm::quat(rotation));
    }

    [[nodiscard]] glm::mat4 modelMatrix() const
    {
        auto transMat = glm::translate(glm::mat4{1.0f}, position);
        auto rotMat = glm::mat4_cast(glm::quat(rotation));
        auto scaleMat = glm::scale(glm::mat4{1.0f}, glm::vec3{scale});
        return transMat * rotMat * scaleMat;
    }

    [[nodiscard]] glm::vec3 front() const
    {
        return rotationMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
    }

    [[nodiscard]] glm::vec3 right() const
    {
        return rotationMatrix() * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    [[nodiscard]] glm::vec3 up() const
    {
        return rotationMatrix() * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    static nlohmann::json serialize(const Transform& transform);
    static Transform deserialize(const nlohmann::json& j);
};

Transform operator+(const Transform& lhs, const Transform& rhs);




#endif //FPSFROMSCRATCH_TRANSFORM_H
