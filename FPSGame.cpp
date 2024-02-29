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
#include <thread>


using namespace std::chrono_literals;


static float speed = 10.0f;


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
    mTextureShader{Util::getShaderPath("model.vert").c_str(), Util::getShaderPath("model.frag").c_str()},
    mColorShader{Util::getShaderPath("model.vert").c_str(), Util::getShaderPath("color.frag").c_str()},
    mSphere{Geometry::makeSphere()},
    mBox{Geometry::makeBox()},
    mBoxWFrame{Geometry::makeBoxWireframe()},
    mGroundPlane{Geometry::makePlane(100.0f, 100.0f, {Texture::loadFromFile(Util::getAssetPath("textures/bathroom-tiling.jpg"))})},
    mSkullModel(Util::getAssetPath("models/Skull/12140_Skull_v3_L2.obj"), true, false)
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
        mVelo.z = -speed;
    };
    mPressHandlers[GLFW_KEY_D] = [this]() {
        mVelo.x = speed;
    };
    mPressHandlers[GLFW_KEY_S] = [this]() {
        mVelo.z = speed;
    };
    mPressHandlers[GLFW_KEY_A] = [this]() {
        mVelo.x = -speed;
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


    makeSkullWall(10, 10);
}

FPSGame::~FPSGame()
{
    Util::shutdownGraphics();
}

void FPSGame::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    FPSGame& self = FPSGame::instance();
    self.mWidth = width;
    self.mHeight = height;
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


    ImGui::Begin("Settings");
    {
        static glm::vec3 size{1.f, 1.f, 1.f};
        if (ImGui::SliderFloat3("BoxDimensions", &size[0], 0.f, 100.f)) {
            mBoxWFrame = Geometry::makeBoxWireframe(size);
        }
    }
    {
        if (ImGui::Button(mUseColorShader ? "Use Texture shader" : "Use Color shader")) {
            mUseColorShader = !mUseColorShader;
        }
    }
    ImGui::End();

    ImGui::Begin("Entities");
    if (ImGui::Button("Add skull")) {
        Entity skull;
        skull.model = &mSkullModel;
        skull.name = "skull" + std::to_string(mEntities.size());
        mEntities.push_back(skull);
    }
    if (ImGui::Button("Add eyeball")) {
        Entity eye;
        if (mEyeModel == nullptr) {
            mEyeModel = new Model(Util::getAssetPath("models/Eye/eyeball.obj"), true, false);
        }
        eye.model = mEyeModel;
        eye.name = "Eye" + std::to_string(mEntities.size());
        mEntities.push_back(eye);
    }
    for (auto& entity : mEntities) {
        entity.update();
    }
    ImGui::End();

    ImGui::Begin("Player");
    mPlayer.onGui();
    ImGui::End();

    const float t = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now().time_since_epoch()).count();
    const float yaw = glm::radians(20 * std::sin(3 * t));
    for (auto& entity : mEntities) {
        entity.yaw = yaw;
    }
}

void FPSGame::render()
{
    Shader& shader = mUseColorShader ? mColorShader : mTextureShader;

    shader.use();

    glm::mat4 model(1);
    glm::mat4 projection(1);
    model = glm::rotate(model, mAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 view = mCamera.getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.1f, 100.0f);

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    mSphere.draw(shader, mDrawMode);

    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    mBox.draw(shader, mDrawMode);

    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    mBoxWFrame.draw(shader, mDrawMode);

    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);

    for (auto& entity : mEntities) {
        entity.draw(shader);
    }

    model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
    shader.setMat4("model", model);
    mGroundPlane.draw(shader);

    mPlayer.draw(shader);
}



void FPSGame::run()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        const auto dt = mTimer.tick();

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        update(dt);

        ImGui::Render();
        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mWindow);
    }
}

void FPSGame::makeSkullWall(int width, int height)
{
    glm::vec3 startPos{};
    const glm::vec3 scale{0.1f, 0.1f, 0.1f};
    const glm::vec3 dw{2.f, 0.0f, 0.0f};
    const glm::vec3 dh{0.0f, 2.5f, 0.0f};

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Entity skull;
            skull.model = &mSkullModel;
            skull.pitch = glm::radians(-90.f);
            skull.pos = static_cast<float>(x) * dw + static_cast<float>(y) * dh;
            skull.scale = scale;
            mEntities.push_back(skull);
        }
    }
}


