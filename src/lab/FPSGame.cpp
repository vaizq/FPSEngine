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
#include <algorithm>
#include <memory>
#include <numeric>
#include <thread>
#include <nlohmann/json.hpp>
#include "engine/RayCast.hpp"
#include "engine/DebugRenderer.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Renderer.hpp"
#include "engine/Random.hpp"
#include <glm/mat4x4.hpp>


using namespace std::chrono_literals;


void FPSGame::startup() {
}

void FPSGame::shutdown() {
    saveScene();
    gRenderer.shutdown();
}

FPSGame::FPSGame() {
    loadScene();
    InputManager::instance().keyPressHandlers[GLFW_KEY_ESCAPE] = [this]() {
        auto window = gRenderer.getWindow();
        int curMode = glfwGetInputMode(window, GLFW_CURSOR);
        if (curMode == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            player->enableInput();
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            player->disableInput();
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

void FPSGame::update(Duration dt)
{
    mScene->forEach([dt](GameObject& obj) {
        obj.update(dt);
    });

    displayFPS(dt);

    static bool useTracking{false};
    ImGui::Begin("Settings");
    {
        if (ImGui::Button("Order")) {
            mScene->forEach([](GameObject& entity) {
                if (entity.skinnedModel) {
                    entity.skinnedModel->animTime = 0;
                }
            });
        }

        if (ImGui::Button("Disorder")) {
            mScene->forEach([](GameObject& entity) {
                if (entity.skinnedModel) {
                    entity.skinnedModel->animTime = randomFloat() * entity.skinnedModel->model.animations[0].duration;
                }
            });
        }


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

    gRenderer.setView(mCamera.getViewMatrix());

    // Draw different coordinate systems
    if (mDrawCoordinateSystems) {
        colorShader.use();
        colorShader.setMat4("view", gRenderer.getView());
        colorShader.setMat4("projection", gRenderer.getProjection());

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

                            const auto modelBase = parentTransform * entity.transform.modelMatrix();

                            colorShader.setMat4("model", modelBase);

                            drawCoordinates();

                            if (entity.skinnedModel) {
                                Transform t{modelBase * 
                                            entity.skinnedModel->skeleton.joints[1].finalTransform};
                                t.scale = glm::vec3{1.0f};
                                colorShader.setMat4("model", t.modelMatrix());
                                drawCoordinates();
                            }
                        });
    }

    // Draw scene
    {
        shader.use();
        shader.setMat4("view", gRenderer.getView());
        shader.setMat4("projection", gRenderer.getProjection());
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

    for (int i = 0; i < 10; i++) {
        gDebugRenderer.addSphere(glm::vec3{i, 0, 0}, 1, Util::red, (1 + i) * 1s);
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

    auto drone = std::make_unique<Drone>(mCamera);
    drone->name = "drone";
    drone->parent = mScene.get();
    player = drone.get();

    auto monster = std::make_unique<GameObject>();
    monster->name = "monster";
    monster->skinnedModel = ResourceManager::instance().getSkinnedModel("monster");
    monster->parent = mScene.get();

    auto soldier = std::make_unique<GameObject>();
    soldier->name = "soldier";
    soldier->skinnedModel = ResourceManager::instance().getSkinnedModel("soldier");
    soldier->parent = mScene.get();

    /*
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            auto m = std::make_unique<GameObject>();
            m->name = std::format("monster[{}][{}]", i, j);
            m->skinnedModel = ResourceManager::instance().getSkinnedModel("monster");
            m->skinnedModel->animTime = randomFloat() * m->skinnedModel->model.animations[0].duration;
            m->parent = mScene.get();
            m->transform.position.x = i * 2;
            m->transform.position.z = j * 2;
            mScene->children.push_back(std::move(m));
        }
    }
    */

    auto light = std::make_unique<Light>();
    light->name = "light";
    light->parent = mScene.get();

    mScene->children.push_back(std::move(drone));
    mScene->children.push_back(std::move(monster));
    mScene->children.push_back(std::move(soldier));
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
