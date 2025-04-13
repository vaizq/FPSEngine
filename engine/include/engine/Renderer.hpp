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
#include <glm/ext/matrix_clip_space.hpp>


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

    float getFov() const {
        return fov;
    }

    void setFov(float fov) {
        this->fov = fov;
        projection = glm::perspective(glm::radians(fov), (float)windowSize.x / (float)windowSize.y, 0.1f, 400.0f);
    }

    void usePerspective() {
        projection = glm::perspective(glm::radians(fov), (float)windowSize.x / (float)windowSize.y, 0.1f, 400.0f);
    }

    void useOrthogonal(glm::vec4 params) {
        projection = glm::ortho(0.0f, 400.0f, 0.0f, 400.0f, -1.0f, 400.0f);
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
    float fov{45.0f};
    glm::mat4 projection;
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
