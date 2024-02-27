//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_FPSGAME_H
#define FPSFROMSCRATCH_FPSGAME_H


#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "Shader.h"
#include "Mesh.h"
#include <memory>
#include <functional>
#include <map>
#include "Camera.h"
#include <chrono>
#include "DeltaTimer.hpp"
#include "Util.h"
#include "Model.h"
#include "Player.h"


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

    GLFWwindow* mWindow;
    int mWidth{};
    int mHeight{};
    DeltaTimer<Clock> mTimer;
    Shader mTextureShader;
    Shader mColorShader;
    Mesh mSphere;
    Mesh mBox;
    Mesh mBoxWFrame;
    glm::vec3 mVelo;
    glm::vec3 mPos;
    std::map<int, std::function<void()>> mPressHandlers;
    std::map<int, std::function<void()>> mReleaseHandlers;
    GLenum mDrawMode = GL_TRIANGLES;
    float mAngle = 0.0f;
    bool mRotate{false};
    int mNVertices{100};
    Camera mCamera;
    bool mUseCamera{false};
    Model mAk47Model;
    Model mBackPackModel;
    bool mUseColorShader{true};
    Player mPlayer;
    Mesh mGroundPlane;
};


#endif //FPSFROMSCRATCH_FPSGAME_H
