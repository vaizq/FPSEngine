#ifndef MATH_HPP
#define MATH_HPP

#include <glm/glm.hpp>


static float angleBetween(const glm::vec3& a, const glm::vec3& b) {
    return std::atan2(glm::length(glm::cross(a, b)), glm::dot(a, b));
}

#endif
