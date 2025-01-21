#include "Renderer.hpp"
#include <glm/glm.hpp>
#include <stdexcept>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <glm/ext/matrix_clip_space.hpp>


static const std::string shadersDir = SRC_DIR "/shaders";


std::string shaderPath(const std::string& shaderName)
{
    return shadersDir + '/' + shaderName;
}


void Renderer::startup(const char* windowName) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowSize.x, windowSize.y, windowName, nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("Failed to create window");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Unlimited FPS

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize glad");
    }

    glEnable(GL_DEPTH_TEST);

    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    shaders.insert({ShaderID::Basic, Shader{shaderPath("basic.vert").c_str(), shaderPath("model.frag").c_str()}});
    shaders.insert({ShaderID::Model, Shader{shaderPath("model.vert").c_str(), shaderPath("model.frag").c_str()}});
    shaders.insert({ShaderID::Color, Shader{shaderPath("color.vert").c_str(), shaderPath("color.frag").c_str()}});
}

void Renderer::shutdown() {
    shaders.clear();
    glfwTerminate();
}

void Renderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    gRenderer.windowSize = {width, height};
}

glm::mat4 Renderer::getProjection() const {
    return glm::perspective(glm::radians(45.0f), (float)windowSize.x / (float)windowSize.y, 0.1f, 400.0f);
}
