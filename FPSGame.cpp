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
#include <nlohmann/json.hpp>


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
    mSkullModel(Util::getAssetPath("models/Skull/12140_Skull_v3_L2.obj")),
    mEyeModel(Util::getAssetPath("models/Eye/eyeball.obj"))
{
    glfwGetWindowSize(mWindow, &mWidth, &mHeight);
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    glfwSetKeyCallback(mWindow, keyCallback);
    glfwSetCursorPosCallback(mWindow, cursorCallback);

    Util::initImgui(mWindow); // Initialize ImGui after my callbacks are installed

    loadScene();

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
}

FPSGame::~FPSGame()
{
    saveScene();
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
    mCamera.translate(-mVelo.z * dt.count(), mVelo.x * dt.count());

    displayFPS(dt);

    ImGui::Begin("Settings");
    {
        if (ImGui::Button(mUseColorShader ? "Use Texture shader" : "Use Color shader")) {
            mUseColorShader = !mUseColorShader;
        }
    }
    ImGui::End();

    ImGui::Begin("Entities");
    mScene->forEach([](Entity& entity) {
        ImGui::PushID(&entity);
        ImGui::SeparatorText(entity.name.c_str());
        entity.onGUI();
        ImGui::PopID();
    });
    ImGui::End();

    ImGui::Begin("Player");
    ImGui::End();
}

void FPSGame::render()
{
    Shader& shader = mUseColorShader ? mColorShader : mTextureShader;

    shader.use();

    glm::mat4 model(1);
    glm::mat4 projection(1);
    glm::mat4 view = mCamera.getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.1f, 100.0f);

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    mScene->draw(shader);
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

void FPSGame::buildScene()
{
    auto makeEye = [this](std::string name) {
        auto eye = std::make_unique<Scene>();
        eye->entity.model = &mEyeModel;
        eye->entity.name = std::move(name);
        return eye;
    };

    auto skullWithEyes = std::make_unique<Scene>();
    skullWithEyes->entity.model = &mSkullModel;
    skullWithEyes->entity.name = "Skull";
    skullWithEyes->children.push_back(makeEye("left eye"));
    skullWithEyes->children.push_back(makeEye("right eye"));

    mScene = std::make_unique<Scene>();
    mScene->entity.name = "World";
    mScene->children.push_back(std::move(skullWithEyes));
}


static constexpr auto sceneFile{"scene.json"};

void FPSGame::loadScene()
{
    buildScene();

    // Load transformations from a file
    try {
        std::ifstream ifs(sceneFile);
        const auto json = nlohmann::json::parse(ifs);
        mScene->forEach([&json](Entity& entity) {
            const auto& name = entity.name;
            entity.position.x = json[name]["posX"];
            entity.position.y = json[name]["posY"];
            entity.position.z = json[name]["posZ"];
            entity.yaw = json[name]["yaw"];
            entity.pitch = json[name]["pitch"];
            entity.scale = json[name]["scale"];
        });
    }
    catch(const std::exception& e) {
        std::cerr << "Unable to load scene: " << e.what() << std::endl;
    }
}

void FPSGame::saveScene()
{
    nlohmann::json json;
    mScene->forEach([&json](Entity& entity) {
        const auto& name = entity.name;
        json[name]["posX"] = entity.position.x;
        json[name]["posY"] = entity.position.y;
        json[name]["posZ"] = entity.position.z;
        json[name]["yaw"] = entity.yaw;
        json[name]["pitch"] = entity.pitch;
        json[name]["scale"] = entity.scale;
    });

    try {
        std::ofstream ofs{sceneFile};
        ofs << json.dump();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save scene: " << e.what() << std::endl;
    }
}

