//
// Created by vaige on 4.3.2024.
//

#ifndef FPSFROMSCRATCH_TRANSFORM_H
#define FPSFROMSCRATCH_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <nlohmann/json_fwd.hpp>



static constexpr glm::vec3 worldUp{0.0f, 1.0f, 0.0f};


// Transform defines a coordinate system
struct Transform
{
    glm::vec3 position{};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};

    Transform() = default;

    Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    : position{position}, rotation{rotation}, scale{scale}
    {}

    explicit Transform(const glm::mat4& modelMatrix)
    {
        glm::vec3 skew{};
        glm::vec4 perspective{};
        glm::decompose(modelMatrix, scale, rotation, position, skew, perspective);
    }

    [[nodiscard]] glm::mat4 rotationMatrix() const
    {
        return glm::mat4_cast(glm::quat(rotation));
    }

    [[nodiscard]] glm::mat4 modelMatrix() const
    {
        auto m = glm::translate(glm::mat4{1.0f}, position);
        m *= glm::mat4_cast(glm::quat(rotation));
        return glm::scale(m, glm::vec3{scale});
    }

    [[nodiscard]] glm::vec3 front() const
    {
        return rotation * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
    }

    [[nodiscard]] glm::vec3 right() const
    {
        return rotation * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    [[nodiscard]] glm::vec3 up() const
    {
        return rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    static nlohmann::json serialize(const Transform& transform);
    static Transform deserialize(const nlohmann::json& j);
};

Transform operator+(const Transform& lhs, const Transform& rhs);

Transform lerp(const Transform& x, const Transform& y, float a);


#endif //FPSFROMSCRATCH_TRANSFORM_H
