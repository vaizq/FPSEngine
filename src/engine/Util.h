//
// Created by vaige on 20.2.2024.
//

#ifndef FPSFROMSCRATCH_UTIL_H
#define FPSFROMSCRATCH_UTIL_H


#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <nlohmann/json_fwd.hpp>

namespace Util
{
    std::string getShaderPath(const std::string& shaderName);
    std::string getAssetPath(const std::string& assetName);

    // Extract filename from a filepath. /some/path/folder/filename.extension -> filename
    std::string extractFilename(const std::string& filepath);


    GLFWwindow* initGraphics(int windowWidth = 640, int windowHeight = 480, const std::string& windowName = "GLFW Window");
    void shutdownGraphics();
    void initImgui(GLFWwindow* window);


    constexpr glm::vec3 red{1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 green{0.0f, 1.0f, 0.0f};
    constexpr glm::vec3 blue{0.0f, 0.0f, 1.0f};

    nlohmann::json serialize(const glm::vec3& v);
    namespace glm::vec3
    {
        ::glm::vec3 deserialize(const nlohmann::json &j);
    }
}

#endif //FPSFROMSCRATCH_UTIL_H
