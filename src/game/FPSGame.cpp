//
// Created by vaige on 19.2.2024.
//

#include "FPSGame.h"
#include <iostream>
#include "../core/Util.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../core/Geometry.h"
#include <imgui.h>
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include <algorithm>
#include <numeric>
#include <thread>
#include <nlohmann/json.hpp>
#include "Tracker.h"
#include "../core/RayCast.h"
#include "Player.h"


using namespace std::chrono_literals;


static float speed = 30.0f;


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
    mSkullModel(Util::getAssetPath("models/Skull/skull.obj")),
    mEyeModel(Util::getAssetPath("models/Eye/eyeball.obj")),
    mAK47Model(Util::getAssetPath("models/ak47/ak47.obj")),
    mGroundPlane(Geometry::makePlane(100.f, 100.f, {Texture::loadFromFile(Util::getAssetPath("textures/bathroom-tiling.jpg"))})),
    mSphereMesh(Geometry::makeSphere(300)),
    mCrosshire(Geometry::makeCrosshire())
{
    glfwGetWindowSize(mWindow, &mWidth, &mHeight);
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    InputManager::registerCallbacks(mWindow); // Initialize InputManager
    Util::initImgui(mWindow); // Initialize ImGui after my callbacks are installed

    loadScene();

    // Get some entities from scene
    mSkull = mScene->findChildren("Skull");
    mLeftEye = mScene->findChildren("leftEye");
    mRightEye = mScene->findChildren("rightEye");

    if (mSkull == nullptr || mLeftEye == nullptr || mRightEye == nullptr) {
        std::cerr << "Unable to find all game objects" << std::endl;
    }


    // Switch drawing mode when R is pressed
    InputManager::instance().pressHandlers[GLFW_KEY_M] = [this]() {
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

    InputManager::instance().pressHandlers[GLFW_KEY_ESCAPE] = [this]() {
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

    InputManager::instance().pressHandlers[GLFW_KEY_S] = [this]() {
        mVelo.z = speed;
    };
    InputManager::instance().pressHandlers[GLFW_KEY_W] = [this]() {
        mVelo.z = -speed;
    };
    InputManager::instance().pressHandlers[GLFW_KEY_D] = [this]() {
        mVelo.x = speed;
    };
    InputManager::instance().pressHandlers[GLFW_KEY_A] = [this]() {
        mVelo.x = -speed;
    };

    InputManager::instance().releaseHandlers[GLFW_KEY_S] = [this]() {
        if (mVelo.z > 0.0f) {
            mVelo.z = 0.0f;
        }
    };
    InputManager::instance().releaseHandlers[GLFW_KEY_W] = [this]() {
        if (mVelo.z < 0.0f) {
            mVelo.z = 0.0f;
        }
    };
    InputManager::instance().releaseHandlers[GLFW_KEY_D] = [this]() {
        if (mVelo.x > 0.0f) {
            mVelo.x = 0.0f;
        }
    };
    InputManager::instance().releaseHandlers[GLFW_KEY_A] = [this]() {
        if (mVelo.x < 0.0f) {
            mVelo.x = 0.0f;
        }
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
    mScene->forEach([dt](GameObject& obj) {
        obj.update(dt);
    });

    displayFPS(dt);

    static bool useTracking{false};
    ImGui::Begin("Settings");
    {
        if (ImGui::Button(mUseColorShader ? "Use Texture shader" : "Use Color shader")) {
            mUseColorShader = !mUseColorShader;
        }
        if (ImGui::Button(useTracking ? "Disable tracking" : "Enable tracking")) {
            useTracking = !useTracking;
        }
        if (ImGui::Button(mDrawCoordinateSystems ? "Hide coordinate systems" : "Draw coordinate systems")) {
            mDrawCoordinateSystems = !mDrawCoordinateSystems;
        }
        if (ImGui::Button(mDrawBounds ? "Hide bounds" : "Draw bounds")) {
            mDrawBounds = !mDrawBounds;
        }

        ImGui::SliderFloat("Crosshire scale", &mCrosshireScale, 0.0001f, 1.0f);

        if (ImGui::Button(mUseDebugCamera ? "Use FPS camera" : "Use debug camera")) {
            mUseDebugCamera = !mUseDebugCamera;
        }

        if (mUseDebugCamera) {
            MyGui::InputTransform("DebugCamera", mDebugCamera.getTransform());
        }
    }
    ImGui::End();

    ImGui::Begin("Entities");
    mScene->forEach([](GameObject& entity) {
        ImGui::PushID(&entity);
        ImGui::SeparatorText(entity.name.c_str());
        entity.onGUI();
        ImGui::PopID();
    });
    ImGui::End();

    if (useTracking) {
        alignTo(mCamera.getPosition(), *mLeftEye, mSkull->transform.modelMatrix());
        alignTo(mCamera.getPosition(), *mRightEye, mSkull->transform.modelMatrix());
    }

    const RayCast ray{mCamera.getPosition(), mCamera.getFront()};
    mTarget = nullptr;
    auto closest = glm::vec3{std::numeric_limits<float>::max()};
    // Find closest object intersecting with a raycast fired from main camera
    mScene->forEach([this, &ray, &closest](GameObject& entity, const glm::mat4& parentTransform) {
        // Transform local entity bounds into world coordinates
        BoundingVolume bounds{Transform{parentTransform * entity.transform.modelMatrix() * entity.bounds.transform.modelMatrix()}, entity.bounds.shape};

        auto intersectionPoint = intersects(ray, bounds);
        if (intersectionPoint != std::nullopt && glm::length(*intersectionPoint - ray.startPosition) < glm::length(closest - ray.startPosition)) {
            mTarget = &entity;
            closest = *intersectionPoint;
        }
    });
}

void FPSGame::render()
{
    Shader& shader = mUseColorShader ? mColorShader : mTextureShader;

    glm::mat4 projection(1);
    glm::mat4 view = mUseDebugCamera ? mDebugCamera.getViewMatrix() : mCamera.getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.1f, 100.0f);

    // Draw different coordinate systems
    if (mDrawCoordinateSystems) {
        mColorShader.use();
        mColorShader.setMat4("view", view);
        mColorShader.setMat4("projection", projection);

        mScene->forEach([this](GameObject &entity, const glm::mat4 &parentTransform)
                        {
                            auto drawCoordinates = [this]()
                            {
                                static Mesh xAxis = Geometry::makeXAxis();
                                static Mesh yAxis = Geometry::makeYAxis();
                                static Mesh zAxis = Geometry::makeZAxis();

                                mColorShader.setVec3("color", Util::red);
                                xAxis.draw(mColorShader, GL_LINES);

                                mColorShader.setVec3("color", Util::green);
                                yAxis.draw(mColorShader, GL_LINES);

                                mColorShader.setVec3("color", Util::blue);
                                zAxis.draw(mColorShader, GL_LINES);
                            };

                            mColorShader.setMat4("model", glm::scale(parentTransform * entity.transform.modelMatrix(), glm::vec3{5.f}));
                            drawCoordinates();
                        });
    }

    // Draw bounds
    if (mDrawBounds) {
        mColorShader.use();
        mColorShader.setMat4("view", view);
        mColorShader.setMat4("projection", projection);

        mScene->forEach([this](GameObject& entity, const glm::mat4& parentTransform) {

            if (auto shape = std::get_if<Sphere>(&entity.bounds.shape)) {
                auto finalTransform = parentTransform * entity.transform.modelMatrix() * entity.bounds.transform.modelMatrix();
                finalTransform = glm::scale(finalTransform, glm::vec3(shape->radius));
                mColorShader.setMat4("model", finalTransform);
                if (mTarget == &entity) {
                    mColorShader.setVec3("color", Util::red);
                }
                else {
                    mColorShader.setVec3("color", Util::green);
                }
                mSphereMesh.draw(mColorShader, GL_LINES);
            }
        });
    }

    // Draw scene
    {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        mScene->forEach([&shader](GameObject &entity, const glm::mat4 &transform)
                        {
                            shader.setMat4("model", transform * entity.transform.modelMatrix());
                            if (entity.model != nullptr) {
                                entity.model->draw(shader);
                            }
                        });

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        mGroundPlane.draw(shader);
    }


    // Draw crosshire
    {
        glm::mat4 model(1.0f);
        const float k = static_cast<float>(mWidth) / static_cast<float>(mHeight);
        model = glm::scale(model, glm::vec3(mCrosshireScale, mCrosshireScale * k, 1.0f));
        mColorShader.use();
        mColorShader.setMat4("view", glm::mat4{1.0f});
        mColorShader.setMat4("projection", glm::mat4{1.0f});
        mColorShader.setMat4("model", model);
        mColorShader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));

        mCrosshire.draw(mColorShader, GL_LINES);
    }
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
        auto eye = std::make_unique<GameObject>();
        eye->name = std::move(name);
        eye->model = &mEyeModel;
        return eye;
    };

    auto skullWithEyes = std::make_unique<GameObject>();
    skullWithEyes->name = "Skull";
    skullWithEyes->model = &mSkullModel;
    skullWithEyes->bounds.shape = Sphere{1.0f};
    skullWithEyes->children.push_back(makeEye("leftEye"));
    skullWithEyes->children.push_back(makeEye("rightEye"));

    auto player = std::make_unique<Player>(mCamera, &mAK47Model);

    mScene = std::make_unique<GameObject>();
    mScene->name = "World";
    mScene->children.push_back(std::move(skullWithEyes));
    mScene->children.push_back(std::move(player));
}


static constexpr auto sceneFile{"scene.json"};

void FPSGame::loadScene()
{
    buildScene();

    // Load transformations from a file
    try {
        std::ifstream ifs(sceneFile);
        const auto json = nlohmann::json::parse(ifs);
        mScene->forEach([&json](GameObject& entity) {
            const auto& name = entity.name;
            if (json.contains(name)) {
                GameObject::deserialize(json[name], entity);
            }
        });
    }
    catch(const std::exception& e) {
        std::cerr << "Unable to load scene: " << e.what() << std::endl;
    }
}

void FPSGame::saveScene()
{
    nlohmann::json json;
    mScene->forEach([&json](GameObject& entity) {
        const auto& name = entity.name;
        json[name] = GameObject::serialize(entity);
    });

    try {
        std::ofstream ofs{sceneFile};
        ofs << json.dump();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save scene: " << e.what() << std::endl;
    }
}

