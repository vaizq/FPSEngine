//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_FPSGAME_H
#define FPSFROMSCRATCH_FPSGAME_H


#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "engine/Shader.hpp"
#include "engine/Mesh.hpp"
#include <memory>
#include <functional>
#include <map>
#include "engine/Camera.hpp"
#include <chrono>
#include "engine/DeltaTimer.hpp"
#include "engine/Util.hpp"
#include "engine/Model.hpp"
#include "engine/GameObject.hpp"
#include "engine/Light.hpp"
#include "Drone.hpp"
#include "engine/Singleton.hpp"


class FPSGame
{
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<float>;

    FPSGame();
    ~FPSGame();

    void startup();
    void shutdown();

    void update(Duration dt);
    void render();
    void run();

private:
    void buildScene();
    void loadScene();
    void saveScene();
    DeltaTimer<Clock> mTimer{};
    Camera mCamera;
    bool mUseColorShader{false};
    bool mDrawCoordinateSystems{false};
    std::unique_ptr<GameObject> mScene;
    Drone* player;
};


#endif //FPSFROMSCRATCH_FPSGAME_H
