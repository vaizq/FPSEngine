//
// Created by vaige on 25.2.2024.
//

#include "Util.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <stdexcept>

#ifndef SRC_DIR
#define SRC_DIR "."
#endif

static constexpr std::string_view assetsDir = SRC_DIR "/assets";
static constexpr std::string_view shadersDir = SRC_DIR "/shaders";


std::string Util::getShaderPath(const std::string& shaderName) {
    return std::string(shadersDir) + '/' + shaderName;
}

std::string Util::getAssetPath(const std::string& assetName) {
    return std::string(assetsDir) + '/' + assetName;
}

GLFWwindow *Util::initGraphics(int windowWidth, int windowHeight, const std::string& windowName)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "FPSGame", nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("Failed to create window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize glad");
    }

    glEnable(GL_DEPTH_TEST);

    return window;
}

void Util::shutdownGraphics()
{
    glfwTerminate();
}

void Util::initImgui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}