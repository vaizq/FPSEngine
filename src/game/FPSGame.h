//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_FPSGAME_H
#define FPSFROMSCRATCH_FPSGAME_H


#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "../core/Shader.h"
#include "../core/Mesh.h"
#include <memory>
#include <functional>
#include <map>
#include "../core/Camera.h"
#include <chrono>
#include "../core/DeltaTimer.hpp"
#include "../core/Util.h"
#include "../core/Model.h"
#include "Player.h"
#include "../core/Entity.h"


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
    Shader mTextureShader;
    Shader mColorShader;
    Camera mCamera;
    Camera mDebugCamera;
    glm::vec3 mVelo{};
    glm::vec3 mPos{};
    std::map<int, std::function<void()>> mPressHandlers;
    std::map<int, std::function<void()>> mReleaseHandlers;
    GLenum mDrawMode = GL_TRIANGLES;
    bool mUseCamera{false};
    bool mUseColorShader{true};
    bool mDrawCoordinateSystems{false};
    bool mDrawBounds{false};
    bool mUseDebugCamera{false};
    float mCrosshireScale{1.0f};
    Model mSkullModel;
    Model mEyeModel;
    Model mAK47Model;
    Scene::Ptr mScene;
    Entity* mSkull{};
    Entity* mLeftEye{};
    Entity* mRightEye{};
    Entity* mPlayer{};
    Entity* mTarget{};
    Mesh mGroundPlane;
    Mesh mSphereMesh;
    Mesh mCrosshire;
};


#endif //FPSFROMSCRATCH_FPSGAME_H