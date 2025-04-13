//
// Created by vaige on 20.2.2024.
//

#ifndef FPSFROMSCRATCH_UTIL_H
#define FPSFROMSCRATCH_UTIL_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace Util {

template <class> inline constexpr bool always_false_v = false;

std::string getShaderPath(const std::string &shaderName);
std::string getAssetPath(const std::string &assetName);

// Extract filename from a filepath. /some/path/folder/filename.extension ->
// filename
std::string extractFilename(const std::string &filepath);

constexpr glm::vec3 red{1.0f, 0.0f, 0.0f};
constexpr glm::vec3 green{0.0f, 1.0f, 0.0f};
constexpr glm::vec3 blue{0.0f, 0.0f, 1.0f};

static glm::mat4 aiMatrix4x4ToGlmMat4(const aiMatrix4x4 &from) {
  glm::mat4 to;
  // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
  to[0][0] = from.a1;
  to[1][0] = from.a2;
  to[2][0] = from.a3;
  to[3][0] = from.a4;
  to[0][1] = from.b1;
  to[1][1] = from.b2;
  to[2][1] = from.b3;
  to[3][1] = from.b4;
  to[0][2] = from.c1;
  to[1][2] = from.c2;
  to[2][2] = from.c3;
  to[3][2] = from.c4;
  to[0][3] = from.d1;
  to[1][3] = from.d2;
  to[2][3] = from.d3;
  to[3][3] = from.d4;
  return to;
}

constexpr glm::vec3 gravity{0.0f, -9.0f, 0.0f};

nlohmann::json serialize(const glm::vec3 &v);
namespace glm::vec3 {
::glm::vec3 deserialize(const nlohmann::json &j);
};
} // namespace Util

#endif // FPSFROMSCRATCH_UTIL_H
