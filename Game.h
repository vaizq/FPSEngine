//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_GAMEBASE_H
#define FPSFROMSCRATCH_GAMEBASE_H

#include <chrono>
#include <string>
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "DeltaTimer.hpp"


// Game setups window, opengl and ImGui
class Game
{
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<float>;

    struct Config
    {
        int windowWidth;
        int windowHeight;
        std::string windowName;
    };

    explicit Game(const Config& cfg = Config{640, 480, "Game"});
    virtual ~Game();

    void run();

    virtual void handleInput() {}
    virtual void update(Duration dt) {}
    virtual void render() {}

protected:
    int mWidth;
    int mHeight;
    GLFWwindow* mWindow;

private:
    DeltaTimer<Clock> mTimer;
};


#endif //FPSFROMSCRATCH_GAMEBASE_H
