///
// Created by vaige on 19.2.2024.
//

#include "FPSGame.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/Geometry.h"
#include <imgui.h>
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include <algorithm>
#include <numeric>
#include <thread>
#include <nlohmann/json.hpp>
#include "Tracker.h"
#include "engine/RayCast.h"
#include "Player.h"
#include "engine/ParticleEmitter.h"
#include "engine/Renderer.h"
#include "EnemyManager.h"
#include "engine/Speaker.h"
#include "drone.h"


using namespace std::chrono_literals;


FPSGame& FPSGame::instance() {
    static std::unique_ptr<FPSGame> self;
    if (self == nullptr) {
        auto window = Util::initGraphics(800, 600, "FPS Game");

        // This takes forever
        ResourceManager::instance().loadAll();

        self = std::unique_ptr<FPSGame>(new FPSGame(window));
    }
    return *self;
}


FPSGame::FPSGame(GLFWwindow* window)
:   mWindow{window},
    mSphereMesh(Geometry::makeSphere(300))
{
    glfwGetWindowSize(mWindow, &mWidth, &mHeight);
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    InputManager::registerCallbacks(mWindow); // Initialize InputManager
    Util::initImgui(mWindow); // Initialize ImGui after my callbacks are installed

    loadScene();

    // Switch drawing mode when R is pressed
    InputManager::instance().keyPressHandlers[GLFW_KEY_M] = [this]() {
        switch (Renderer::drawMode)
        {
            case GL_TRIANGLES:
                Renderer::drawMode = GL_LINES;
                break;
            case GL_LINES:
                Renderer::drawMode = GL_LINE_STRIP;
                break;
            case GL_LINE_STRIP:
                Renderer::drawMode = GL_LINE_LOOP;
                break;
            case GL_LINE_LOOP:
                Renderer::drawMode = GL_POINTS;
                break;
            case GL_POINTS:
                Renderer::drawMode = GL_TRIANGLES;
        }
    };

    InputManager::instance().keyPressHandlers[GLFW_KEY_ESCAPE] = [this]() {
        int curMode = glfwGetInputMode(mWindow, GLFW_CURSOR);
        if (curMode == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            player->enableInput();
        }
        else {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            player->disableInput();
        }
    };
}

FPSGame::~FPSGame()
{
    saveScene();
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
    static float fps{1.0f / dt.count()};
    ImGui::Begin("Status");
    ImGui::Text("FPS %.0f", fps);
    ImGui::End();
}

void FPSGame::onGUI(Duration dt)
{

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

        if (ImGui::Button("Reload shaders")) {
            ResourceManager::instance().reloadShaders();
            printf("shaders reloaded\n");
        }

        static int width{100};
        static int height{100};
        static int gridSize{10};
        static glm::vec3 scale{1.0f};

        ImGui::DragInt("GroundWidth", &width);
        ImGui::DragInt("GroundHeight", &height);
        ImGui::DragInt("GroundGridSize", &gridSize);
        ImGui::DragFloat3("GroundScale", &scale[0], 0.05f);
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
}

void FPSGame::render()
{
    Shader& shader = ResourceManager::instance().getShader(mUseColorShader ? "color" : "model");
    Shader& colorShader = ResourceManager::instance().getShader("color");

    glm::mat4 view = mCamera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)mWidth / (float)mHeight, 0.1f, 400.0f);

    ResourceManager::instance().view = view;
    ResourceManager::instance().projection = projection;

    // Draw different coordinate systems
    if (mDrawCoordinateSystems) {
        colorShader.use();
        colorShader.setMat4("view", view);
        colorShader.setMat4("projection", projection);

        mScene->forEach([this, &colorShader](GameObject &entity, const glm::mat4 &parentTransform)
                        {
                            auto drawCoordinates = [this, &colorShader]()
                            {
                                static Mesh xAxis = Geometry::makeXAxis();
                                static Mesh yAxis = Geometry::makeYAxis();
                                static Mesh zAxis = Geometry::makeZAxis();

                                colorShader.setVec3("color", Util::red);
                                xAxis.draw(colorShader, GL_LINES);

                                colorShader.setVec3("color", Util::green);
                                yAxis.draw(colorShader, GL_LINES);

                                colorShader.setVec3("color", Util::blue);
                                zAxis.draw(colorShader, GL_LINES);
                            };

                            colorShader.setMat4("model", parentTransform * entity.transform.modelMatrix());
                            drawCoordinates();
                        });
    }

    // Draw scene
    {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("lightPosition", mScene->findChildren("light")->transform.position);
        shader.setVec3("cameraPosition", mCamera.getPosition());

        mScene->render(shader);
    }
}


void FPSGame::run()
{
    mScene->forEach([](GameObject& obj) {
        obj.ready();
    });

    float targetFps = 144;
    Duration targetDeltaTime = Duration{1s} / targetFps;
    while (!glfwWindowShouldClose(mWindow))
    {
        const auto dt = mTimer.tick();

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        onGUI(dt);

        update(dt);

        ImGui::Render();
        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mWindow);

        if (targetDeltaTime > dt) {
            std::this_thread::sleep_for(targetDeltaTime - dt);
        }
    }

    mScene->forEach([](GameObject& obj) {
        obj.shutdown();
    });
}

void FPSGame::buildScene()
{
    mScene = std::make_unique<GameObject>();
    mScene->name = "World";

    auto drone = std::make_unique<Drone>(mCamera);
    drone->name = "drone";
    drone->parent = mScene.get();
    player = drone.get();

    auto skeleton = std::make_unique<GameObject>();
    skeleton->name = "skeleton";
    skeleton->model = &ResourceManager::instance().getModel("soldier");
    skeleton->parent = mScene.get();

    auto light = std::make_unique<Light>();
    light->name = "light";
    light->parent = mScene.get();

    mScene->children.push_back(std::move(drone));
    mScene->children.push_back(std::move(skeleton));
    mScene->children.push_back(std::move(light));
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
