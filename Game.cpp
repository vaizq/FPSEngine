//
// Created by vaige on 19.2.2024.
//

#include "Game.h"
#include <stdexcept>

Game::Game(const Config& cfg)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(cfg.windowWidth, cfg.windowHeight, cfg.windowName.c_str(), nullptr, nullptr);
    if (!mWindow) {
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(mWindow);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize glad");
    }

    glEnable(GL_DEPTH_TEST);
}

Game::~Game()
{
    glfwTerminate();
}

void Game::run()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        handleEvent();

        update(mTimer.tick());

        glClearColor(1.00f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render();

        glfwSwapBuffers(mWindow);
    }
}
