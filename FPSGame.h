//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_FPSGAME_H
#define FPSFROMSCRATCH_FPSGAME_H


#include "Game.h"
#include <glm/vec3.hpp>
#include "Shader.h"
#include "Mesh.h"
#include <memory>
#include <functional>
#include <map>
#include "Camera.h"


class FPSGame : public Game
{
public:
    static FPSGame& instance() {
        static FPSGame* self;
        if (self == nullptr) {
            self = new FPSGame;
        }
        return *self;
    }


    void update(Duration dt) override;

    void render() override;

private:
    FPSGame();
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorCallback(GLFWwindow* window, double xPos, double yPos);

    Shader mShader;
    std::unique_ptr<Mesh> mMesh;
    Mesh mSphere;
    glm::vec3 mVelo;
    glm::vec3 mPos;
    std::map<int, std::function<void()>> mPressHandlers;
    std::map<int, std::function<void()>> mReleaseHandlers;
    GLenum mDrawMode = GL_TRIANGLES;
    float angle = 0.0f;
    bool mRotate{false};
    int mNVertices{100};
    Camera mCamera;
};


#endif //FPSFROMSCRATCH_FPSGAME_H
