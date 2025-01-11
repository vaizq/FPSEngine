#ifndef RENDERER_H
#define RENDERER_H

#include "Singleton.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>



class Renderer : public Singleton<Renderer> 
{
public:
    void startup(const char* windowName);
    void shutdown();
    GLFWwindow* getWindow() { return window; }
    glm::ivec2 getWindowSize() { return windowSize; }
    glm::mat4 getProjection() const;
    void setView(const glm::mat4& view) { this->view = view; }
    glm::mat4 getView() const { return view; }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window;
    glm::ivec2 windowSize{800, 600};
    glm::mat4 view;
};


static Renderer& gRenderer = Renderer::instance();


#endif
