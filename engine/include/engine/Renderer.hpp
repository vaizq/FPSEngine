#ifndef RENDERER_H
#define RENDERER_H

#include "Singleton.hpp"
#include "Shader.hpp"
#include <map>
#include <deque>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>


class ShaderHandle;

class Renderer : public Singleton<Renderer> 
{
    friend ShaderHandle;
public:
    enum class ShaderID {
        Basic,
        Model,
        Color
    };

    static std::string to_string(ShaderID id) {
        switch (id) {
            case ShaderID::Basic:
                return "Basic";
            case ShaderID::Model:
                return "Model";
            case ShaderID::Color:
                return "Color";
        }
    }

    void startup(const char* windowName);
    void shutdown();
    GLFWwindow* getWindow() { return window; }
    glm::ivec2 getWindowSize() { return windowSize; }
    glm::mat4 getProjection() const;
    void setView(const glm::mat4& view) { this->view = view; }
    glm::mat4 getView() const { return view; }

    void reloadShaders() {
        for (auto& [name, shader] : shaders) {
            shader.reload();
        }
    }

private:
    Shader& pushShader(ShaderID id) {
        shaderStack.push_back(id);
        Shader& shader = shaders.at(id);
        shader.use();
        return shader;
    }

    void popShader() {
        shaderStack.pop_back();
        if (!shaderStack.empty()) {
            shaders.at(shaderStack.back()).use();
        }
    }

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window;
    glm::ivec2 windowSize{800, 600};
    glm::mat4 view;
    std::map<ShaderID, Shader> shaders; 
    std::deque<ShaderID> shaderStack;
};


static Renderer& gRenderer = Renderer::instance();


class ShaderHandle {
public:
    ShaderHandle(Renderer::ShaderID id)
    : mId{id}, mShader{gRenderer.pushShader(mId)}
    {}

    ~ShaderHandle() {
        gRenderer.popShader();
    }

    Shader& shader() {
        return mShader;
    }

private:
    Renderer::ShaderID mId;
    Shader& mShader;
};


#endif
