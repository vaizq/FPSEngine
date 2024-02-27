//
// Created by vaige on 19.2.2024.
//

#include "FPSGame.h"
#include <iostream>
#include "Util.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Geometry.h"
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <algorithm>
#include <numeric>


using namespace std::chrono_literals;


void FPSGame::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    FPSGame& self = FPSGame::instance();
    self.mWidth = width;
    self.mHeight = height;
}

static std::vector<Vertex> squareMesh()
{
    return {
        Vertex{glm::vec3{0.5f, 0.5f, 0.0f}, glm::vec3{}, glm::vec2{0.5f, 0.5f}},
        Vertex{glm::vec3{0.5f, -0.5f, 0.0f}, glm::vec3{}, glm::vec2{0.5f, -0.5f}},
        Vertex{glm::vec3{-0.5f, -0.5f, 0.0f}, glm::vec3{}, glm::vec2{-0.5f, -0.5f}},
        Vertex{glm::vec3{-0.5f, 0.5f, 0.0f}, glm::vec3{}, glm::vec2{-0.5f, 0.5f}}
    };
}

static std::vector<Vertex> vertices = squareMesh();

static std::vector<unsigned int> indices = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
};

FPSGame& FPSGame::instance() {
    static std::unique_ptr<FPSGame> self;
    if (self == nullptr) {
        auto window = Util::initGraphics(800, 600, "FPS Game");
        self = std::unique_ptr<FPSGame>(new FPSGame(window));
    }
    return *self;
}

FPSGame::FPSGame(GLFWwindow* window)
:   mWindow{window},
    mShader{Util::getShaderPath("model.glsl").c_str(), Util::getShaderPath("color.glsl").c_str()},
    mSphere{Geometry::makeSphere()},
    mBox{Geometry::makeBox()},
    mBoxWFrame{Geometry::makeBoxWireframe()}
{
    glfwGetWindowSize(mWindow, &mWidth, &mHeight);
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    glfwSetKeyCallback(mWindow, keyCallback);
    glfwSetCursorPosCallback(mWindow, cursorCallback);

    Util::initImgui(mWindow); // Initialize ImGui after my callbacks are installed

    // Switch drawing mode when R is pressed
    mPressHandlers[GLFW_KEY_M] = [this]() {
        switch (mDrawMode)
        {
            case GL_TRIANGLES:
                mDrawMode = GL_LINES;
                break;
            case GL_LINES:
                mDrawMode = GL_LINE_STRIP;
                break;
            case GL_LINE_STRIP:
                mDrawMode = GL_LINE_LOOP;
                break;
            case GL_LINE_LOOP:
                mDrawMode = GL_POINTS;
                break;
            case GL_POINTS:
                mDrawMode = GL_TRIANGLES;
        }
    };

    mPressHandlers[GLFW_KEY_ESCAPE] = [this]() {
        int curMode = glfwGetInputMode(mWindow, GLFW_CURSOR);
        if (curMode == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mUseCamera = true;
        }
        else {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mUseCamera = false;
        }
    };

    mPressHandlers[GLFW_KEY_R] = [this]() {
        mRotate = !mRotate;
    };

    mPressHandlers[GLFW_KEY_UP] = [this]() {
        mNVertices *= 2;
        mSphere = Geometry::makeSphere(mNVertices);
    };
    mReleaseHandlers[GLFW_KEY_DOWN] = [this]() {
        mNVertices /= 2;
        mSphere = Geometry::makeSphere(mNVertices);
    };

    mPressHandlers[GLFW_KEY_W] = [this]() {
        mVelo.z = -5.f;
    };
    mPressHandlers[GLFW_KEY_D] = [this]() {
        mVelo.x = 5.f;
    };
    mPressHandlers[GLFW_KEY_S] = [this]() {
        mVelo.z = 5.f;
    };
    mPressHandlers[GLFW_KEY_A] = [this]() {
        mVelo.x = -5.f;
    };

    mReleaseHandlers[GLFW_KEY_W] = [this]() {
        mVelo.z = 0.f;
    };
    mReleaseHandlers[GLFW_KEY_D] = [this]() {
        mVelo.x = 0.f;
    };
    mReleaseHandlers[GLFW_KEY_S] = [this]() {
        mVelo.z = 0.f;
    };
    mReleaseHandlers[GLFW_KEY_A] = [this]() {
        mVelo.x = 0.f;
    };
}

FPSGame::~FPSGame()
{
    Util::shutdownGraphics();
}

void FPSGame::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    FPSGame& self = FPSGame::instance();

    switch (action)
    {
        case GLFW_PRESS:
            if (self.mPressHandlers.contains(key)) {
                self.mPressHandlers[key]();
            }
            break;
        case GLFW_RELEASE:
            if (self.mReleaseHandlers.contains(key)) {
                self.mReleaseHandlers[key]();
            }
            break;
        default:
            break;
    }
}

void FPSGame::cursorCallback(GLFWwindow *window, double xPos, double yPos)
{

    static float prevX{static_cast<float>(xPos)};
    static float prevY{static_cast<float>(yPos)};

    FPSGame& self = FPSGame::instance();
    auto dx = (float) (xPos - prevX);
    auto dy = (float) (yPos - prevY);

    float sensitivity = 0.001f;

    if (self.mUseCamera)
    {
        self.mCamera.rotateYaw(sensitivity * dx);
        self.mCamera.rotatePitch(-sensitivity * dy);
    }

    prevX = static_cast<float> (xPos);
    prevY = static_cast<float> (yPos);
}

static void displayFPS(FPSGame::Duration dt)
{
    static std::vector<float> fpsMeasurements{};
    fpsMeasurements.push_back(1.0f / dt.count());
    static float fps{1.0f / dt.count()};
    if (fpsMeasurements.size() > 10) {
        float total = std::accumulate(fpsMeasurements.begin(), fpsMeasurements.end(), 0.0f);
        fps = total / fpsMeasurements.size();
        fpsMeasurements.clear();
    }
    ImGui::Begin("Status");
    ImGui::Text("FPS %.0f", fps);
    ImGui::End();
}

void FPSGame::update(Duration dt)
{
    if (mRotate)
        mAngle += glm::radians(45.0f) * dt.count();
    mCamera.translate(-mVelo.z * dt.count(), mVelo.x * dt.count());

    displayFPS(dt);


    ImGui::Begin("Box");
    static glm::vec3 size{1.f, 1.f, 1.f};
    if (ImGui::SliderFloat3("BoxDimensions", &size[0], 0.f, 100.f)) {
        mBoxWFrame = Geometry::makeBoxWireframe(size);
    }
    ImGui::End();
}

void FPSGame::render()
{
    mShader.use();

    glm::mat4 model(1);
    glm::mat4 projection(1);
    model = glm::rotate(model, mAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 view = mCamera.getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.1f, 100.0f);

    mShader.setMat4("model", model);
    mShader.setMat4("view", view);
    mShader.setMat4("projection", projection);

    mSphere.draw(mShader, mDrawMode);

    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    mShader.setMat4("model", model);
    mBox.draw(mShader, mDrawMode);

    model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
    mShader.setMat4("model", model);
    mBoxWFrame.draw(mShader, mDrawMode);
}



void FPSGame::run()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        update(mTimer.tick());

        ImGui::Render();
        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mWindow);
    }
}


