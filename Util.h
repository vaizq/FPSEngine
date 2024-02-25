//
// Created by vaige on 20.2.2024.
//

#ifndef FPSFROMSCRATCH_UTIL_H
#define FPSFROMSCRATCH_UTIL_H


#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Util
{
    std::string getShaderPath(const std::string& shaderName);
    std::string getAssetPath(const std::string& assetName);


    GLFWwindow* initGraphics(int windowWidth = 640, int windowHeight = 480, const std::string& windowName = "GLFW Window");
    void shutdownGraphics();
    void initImgui(GLFWwindow* window);
}

#endif //FPSFROMSCRATCH_UTIL_H
