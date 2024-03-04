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
#include "Tracker.h"
#include "RayCast.h"


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
    mEyeModel(Util::getAssetPath("models/Eye/eyeball.obj")),
    mAK47Model(Util::getAssetPath("models/ak47/ak47.obj")),
    mGroundPlane(Geometry::makePlane(100.f, 100.f, {Texture::loadFromFile(Util::getAssetPath("textures/bathroom-tiling.jpg"))})),
    mSphereMesh(Geometry::makeSphere(300)),
    mCrosshire(Geometry::makeCrosshire())
{
    glfwGetWindowSize(mWindow, &mWidth, &mHeight);
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    glfwSetKeyCallback(mWindow, keyCallback);
    glfwSetCursorPosCallback(mWindow, cursorCallback);

    Util::initImgui(mWindow); // Initialize ImGui after my callbacks are installed
    mCamera.setPosition(glm::vec3(0.0f, 3.0f, 0.0f));

    loadScene();


    if (mSkull = mScene->getEntity("Skull"); mSkull == nullptr) {
        std::cerr << "Unable to get skull entity!" << std::endl;
    }

    if (mPlayer = mScene->getEntity("player"); mPlayer == nullptr) {
        std::cerr << "Unable to get player entity!" << std::endl;
    }

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



    mPressHandlers[GLFW_KEY_S] = [this]() {
        mVelo.z = speed;
    };
    mPressHandlers[GLFW_KEY_W] = [this]() {
        mVelo.z = -speed;
    };
    mPressHandlers[GLFW_KEY_D] = [this]() {
        mVelo.x = speed;
    };
    mPressHandlers[GLFW_KEY_A] = [this]() {
        mVelo.x = -speed;
    };

    mReleaseHandlers[GLFW_KEY_S] = [this]() {
        if (mVelo.z > 0.0f) {
            mVelo.z = 0.0f;
        }
    };
    mReleaseHandlers[GLFW_KEY_W] = [this]() {
        if (mVelo.z < 0.0f) {
            mVelo.z = 0.0f;
        }
    };
    mReleaseHandlers[GLFW_KEY_D] = [this]() {
        if (mVelo.x > 0.0f) {
            mVelo.x = 0.0f;
        }
    };
    mReleaseHandlers[GLFW_KEY_A] = [this]() {
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
    // Rotate camera
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


    const RayCast ray{self.mCamera.getPosition(), self.mCamera.getFront()};
    self.mTarget = nullptr;
    auto closest = glm::vec3{std::numeric_limits<float>::max()};
    // Find closest object intersecting with a raycast fired from main camera
    self.mScene->forEach([&self, &ray, &closest](Entity& entity, const glm::mat4& parentTransform) {
        // Transform local entity bounds into world coordinates
        BoundingVolume bounds{Transform{parentTransform * entity.modelViewMatrix() * entity.bounds.transform.modelMatrix()}, entity.bounds.shape};

        if (auto intersectionPoint = intersects(ray, bounds);
        glm::length(*intersectionPoint - ray.startPosition) < glm::length(closest - ray.startPosition)) {
            self.mTarget = &entity;
            closest = *intersectionPoint;
        }
    });

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
    mCamera.FPSTranslate(-mVelo.z * dt.count(), mVelo.x * dt.count());
//    mPlayer->deltaTransform = Transform(mCamera.getPosition() + mCamera.getFront(), mCamera.getYaw(),mCamera.getPitch(), 1.0f);

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

    if (useTracking) {
        setDirection(*mSkull, mCamera.getPosition());
    }
}

void FPSGame::render()
{
    Shader& shader = mUseColorShader ? mColorShader : mTextureShader;

    glm::mat4 projection(1);
    glm::mat4 view = mCamera.getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.1f, 100.0f);

    // Draw different coordinate systems
    if (mDrawCoordinateSystems) {
        mColorShader.use();
        mColorShader.setMat4("view", view);
        mColorShader.setMat4("projection", projection);

        mScene->forEach([this](Entity &entity, const glm::mat4 &parentTransform)
                        {
                            mColorShader.setMat4("model", glm::scale(parentTransform * entity.modelViewMatrix(), glm::vec3{5.f}));
                            static Mesh xAxis = Geometry::makeXAxis();
                            static Mesh yAxis = Geometry::makeYAxis();
                            static Mesh zAxis = Geometry::makeZAxis();

                            mColorShader.setVec3("color", Util::red);
                            xAxis.draw(mColorShader, GL_LINES);

                            mColorShader.setVec3("color", Util::green);
                            yAxis.draw(mColorShader, GL_LINES);

                            mColorShader.setVec3("color", Util::blue);
                            zAxis.draw(mColorShader, GL_LINES);
                        });
    }

    // Draw bounds
    if (mDrawBounds) {
        mColorShader.use();
        mColorShader.setMat4("view", view);
        mColorShader.setMat4("projection", projection);

        mScene->forEach([this](Entity& entity, const glm::mat4& parentTransform) {

            if (auto shape = std::get_if<Sphere>(&entity.bounds.shape)) {
                auto finalTransform = parentTransform * entity.modelViewMatrix() * entity.bounds.transform.modelMatrix();
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

        mScene->forEach([this, &shader](Entity &entity, const glm::mat4 &transform)
                        {
                            shader.setMat4("model", transform * entity.modelViewMatrix());
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
        auto eye = std::make_unique<Scene>();
        eye->entity.model = &mEyeModel;
        eye->entity.name = std::move(name);
        return eye;
    };

    auto skullWithEyes = std::make_unique<Scene>();
    skullWithEyes->entity.model = &mSkullModel;
    skullWithEyes->entity.name = "Skull";
    skullWithEyes->entity.bounds.shape = Sphere{1.0f};
    skullWithEyes->children.push_back(makeEye("left eye"));
    skullWithEyes->children.push_back(makeEye("right eye"));

    auto ak47 = std::make_unique<Scene>();
    ak47->entity.model = &mAK47Model;
    ak47->entity.name = "ak47";

    auto player = std::make_unique<Scene>();
    player->entity.name = "player";
    player->children.push_back(std::move(ak47));

    mScene = std::make_unique<Scene>();
    mScene->entity.name = "World";
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
        mScene->forEach([&json](Entity& entity) {
            const auto& name = entity.name;
            if (json.contains(name)) {
                entity.initialTransform = Transform::deserialize(json[name]["initialTransform"]);
                entity.deltaTransform = Transform::deserialize(json[name]["deltaTransform"]);
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
    mScene->forEach([&json](Entity& entity) {
        const auto& name = entity.name;
        json[name]["initialTransform"] = Transform::serialize(entity.initialTransform);
        json[name]["deltaTransform"] = Transform::serialize(entity.deltaTransform);
    });

    try {
        std::ofstream ofs{sceneFile};
        ofs << json.dump();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save scene: " << e.what() << std::endl;
    }
}

