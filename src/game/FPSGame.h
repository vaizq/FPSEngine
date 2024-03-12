//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_FPSGAME_H
#define FPSFROMSCRATCH_FPSGAME_H


#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "../engine/Shader.h"
#include "../engine/Mesh.h"
#include <memory>
#include <functional>
#include <map>
#include "../engine/Camera.h"
#include <chrono>
#include "../engine/DeltaTimer.hpp"
#include "../engine/Util.h"
#include "../engine/Model.h"
#include "Player.h"
#include "../engine/GameObject.h"
#include "../engine/Light.h"


class FPSGame
{
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<float>;

    static FPSGame& instance();
    ~FPSGame();

    void update(Duration dt);
    void render();
    void run();

private:
    explicit FPSGame(GLFWwindow* window);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorCallback(GLFWwindow* window, double xPos, double yPos);
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
    GLenum mDrawMode = GL_TRIANGLES;
    bool mUseColorShader{true};
    bool mDrawCoordinateSystems{false};
    bool mDrawBounds{false};
    bool mUseDebugCamera{false};
    float mCrosshireScale{0.0f};
    Mesh mGroundPlane;
    Mesh mSphereMesh;
    Mesh mCrosshire;
    std::unique_ptr<GameObject> mScene;
    GameObject* mSkull{};
    GameObject* mLeftEye{};
    GameObject* mRightEye{};
    GameObject* mTarget{};
    Player* mPlayer{};
};


#endif //FPSFROMSCRATCH_FPSGAME_H
