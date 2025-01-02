//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_FPSGAME_H
#define FPSFROMSCRATCH_FPSGAME_H


#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "engine/Shader.h"
#include "engine/Mesh.h"
#include <memory>
#include <functional>
#include <map>
#include "engine/Camera.h"
#include <chrono>
#include "engine/DeltaTimer.hpp"
#include "engine/Util.h"
#include "engine/Model.h"
#include "Player.h"
#include "Enemy.h"
#include "engine/GameObject.h"
#include "engine/Light.h"


class FPSGame
{
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<float>;

    static FPSGame& instance();
    ~FPSGame();

    void onGUI(Duration dt);
    void update(Duration dt);
    void render();
    void run();

private:
    explicit FPSGame(GLFWwindow* window, std::unique_ptr<AudioSource> theme);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void buildScene();
    void loadScene();
    void saveScene();

    GLFWwindow* mWindow;
    int mWidth{};
    int mHeight{};
    DeltaTimer<Clock> mTimer{};
    Camera mCamera;
    Camera mDebugCamera;
    std::map<int, std::function<void()>> mPressHandlers;
    std::map<int, std::function<void()>> mReleaseHandlers;
    bool mUseColorShader{false};
    bool mDrawCoordinateSystems{false};
    bool mDrawBounds{false};
    bool mUseDebugCamera{false};
    float mCrosshireScale{0.0f};
    Mesh mSphereMesh;
    Mesh mCrosshire;
    std::unique_ptr<GameObject> mScene;
    GameObject* mSkull{};
    GameObject* mLeftEye{};
    GameObject* mRightEye{};
    GameObject* mTarget{};
    Player* mPlayer{};
    std::unique_ptr<AudioSource> mTheme;
};


#endif //FPSFROMSCRATCH_FPSGAME_H
