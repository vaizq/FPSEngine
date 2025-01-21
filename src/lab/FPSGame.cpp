///
// Created by vaige on 19.2.2024.
//

#include "FPSGame.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/Geometry.hpp"
#include <imgui.h>
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include <memory>
#include <thread>
#include <nlohmann/json.hpp>
#include "engine/DebugRenderer.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Renderer.hpp"
#include "engine/Random.hpp"
#include <glm/mat4x4.hpp>
#include "Player.hpp"
#include "engine/Renderer.hpp"
#include "Drone.hpp"
#include "Player.hpp"
#include "engine/Terrain.hpp"
#include "engine/Light.hpp"


using namespace std::chrono_literals;


void FPSGame::startup() {
}

void FPSGame::shutdown() {
}

FPSGame::FPSGame() {
    loadScene();
    InputManager::instance().keyPressHandlers[GLFW_KEY_ESCAPE] = [this]() {
        auto window = gRenderer.getWindow();
        int curMode = glfwGetInputMode(window, GLFW_CURSOR);
        if (curMode == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    };
}

FPSGame::~FPSGame() {
    saveScene();
}

static void displayFPS(FPSGame::Duration dt)
{
    if (dt.count() <= (1.0f / 1000)) {
        ImGui::Begin("Status");
        ImGui::Text("FPS +1000");
        ImGui::End();
        return;
    }

    static double fps{144};
    fps = 0.1 * (1.0f / dt.count()) + 0.9 * fps;

    ImGui::Begin("Status");
    ImGui::Text("FPS %f", fps);
    ImGui::End();
}

void sceneGUI(std::unique_ptr<GameObject>& node) {
    if (ImGui::TreeNode(node->name.c_str())) {
        if (ImGui::CollapsingHeader("Settings")) {
            node->onGUI();
        }

        for (auto& child : node->children) {
            sceneGUI(child);
        }
        ImGui::TreePop();
    }
}

void FPSGame::update(Duration dt)
{
    mScene->forEach([dt](GameObject& obj) {
        obj.update(dt);
    });

    displayFPS(dt);

    ImGui::Begin("Editor");

    if (ImGui::CollapsingHeader("Settings")) {
        if (ImGui::Button("Order")) {
            mScene->forEach([](GameObject& entity) {
                if (entity.skinnedModel) {
                    entity.animTime = 0;
                }
            });
        }

        if (ImGui::Button("Disorder")) {
            mScene->forEach([](GameObject& entity) {
                if (entity.skinnedModel) {
                    entity.animTime = randomFloat() * entity.animation->duration;
                }
            });
        }


        if (ImGui::Button(mUseColorShader ? "Use Texture shader" : "Use Color shader")) {
            mUseColorShader = !mUseColorShader;
        }

        if (ImGui::Button(mDrawCoordinateSystems ? "Hide coordinate systems" : "Draw coordinate systems")) {
            mDrawCoordinateSystems = !mDrawCoordinateSystems;
        }

        if (ImGui::Button("Reload shaders")) {
            gRenderer.reloadShaders();
            printf("shaders reloaded\n");
        }
    }

    if (ImGui::CollapsingHeader("Entities")) {
        sceneGUI(mScene);
    }

    ImGui::End();
}

void FPSGame::render()
{
    gRenderer.setView(mCamera.getViewMatrix());


    // Draw different coordinate systems
    if (mDrawCoordinateSystems) {
        ShaderHandle handle(Renderer::ShaderID::Color);

        handle.shader().setMat4("view", gRenderer.getView());
        handle.shader().setMat4("projection", gRenderer.getProjection());

        mScene->forEach([this, &handle](GameObject &entity, const glm::mat4 &parentTransform)
                        {
                            auto drawCoordinates = [this, &handle]()
                            {
                                static Mesh xAxis = Geometry::makeXAxis();
                                static Mesh yAxis = Geometry::makeYAxis();
                                static Mesh zAxis = Geometry::makeZAxis();

                                handle.shader().setVec3("color", Util::red);
                                xAxis.draw(handle.shader(), GL_LINES);

                                handle.shader().setVec3("color", Util::green);
                                yAxis.draw(handle.shader(), GL_LINES);

                                handle.shader().setVec3("color", Util::blue);
                                zAxis.draw(handle.shader(), GL_LINES);
                            };

                            const auto modelBase = parentTransform * entity.transform.modelMatrix();

                            handle.shader().setMat4("model", modelBase);

                            drawCoordinates();

                            if (entity.skinnedModel) {
                                Transform t{modelBase * 
                                            entity.skinnedModel->skeleton.joints[1].finalTransform};
                                t.scale = glm::vec3{1.0f};
                                handle.shader().setMat4("model", t.modelMatrix());
                                drawCoordinates();
                            }
                        }, glm::mat4{1.0f});
    }


    // Draw scene
    {
        ShaderHandle handle(mUseColorShader ? Renderer::ShaderID::Color : Renderer::ShaderID::Model);

        handle.shader().setMat4("view", gRenderer.getView());
        handle.shader().setMat4("projection", gRenderer.getProjection());
        handle.shader().setVec3("lightPosition", mScene->findChildren("light")->transform.position);
        handle.shader().setVec3("cameraPosition", mCamera.getPosition());

        mScene->render(handle.shader());
    }

}


void FPSGame::run()
{
    mScene->forEach([](GameObject& obj) {
        obj.ready();
    });

    for (int i = 0; i < 10; i++) {
        gDebugRenderer.addSphere(glm::vec3{2*i, 0, 0}, 1, Util::red, (1 + i) * 2s);
    }

    float targetFps = 144;
    Duration targetDeltaTime = Duration{1s} / targetFps;
    while (!glfwWindowShouldClose(gRenderer.getWindow()))
    {
        const auto dt = mTimer.tick();

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        update(dt);
        gDebugRenderer.update(dt);

        ImGui::Render();
        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();

        gDebugRenderer.render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(gRenderer.getWindow());

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

    auto plr = std::make_unique<Player>(mCamera);
    plr->name = "player";
    plr->skinnedModel = ResourceManager::instance().getSkinnedModel("soldier");
    plr->parent = mScene.get();

    auto drone = std::make_unique<Drone>(mCamera);
    drone->name = "drone";
    drone->skinnedModel = ResourceManager::instance().getSkinnedModel("soldier");
    drone->parent = mScene.get();

    auto nurse = std::make_unique<GameObject>();
    nurse->name = "nurse";
    nurse->skinnedModel = ResourceManager::instance().getSkinnedModel("nurse");
    nurse->parent = mScene.get();

    auto soldier = std::make_unique<GameObject>();
    soldier->name = "soldier";
    soldier->skinnedModel = ResourceManager::instance().getSkinnedModel("soldier");
    soldier->parent = mScene.get();

    auto light = std::make_unique<Light>();
    light->name = "light";
    light->parent = mScene.get();

    auto terrain = std::make_unique<Terrain>();
    terrain->name = "terrain";
    terrain->parent = mScene.get();

    mScene->children.push_back(std::move(plr));
    mScene->children.push_back(std::move(nurse));
    mScene->children.push_back(std::move(soldier));
    mScene->children.push_back(std::move(light));
    mScene->children.push_back(std::move(drone));
    mScene->children.push_back(std::move(terrain));
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
