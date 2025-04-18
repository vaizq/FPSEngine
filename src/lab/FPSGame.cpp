///
// Created by vaige on 19.2.2024.
//

#include "FPSGame.h"
#include "Drone.hpp"
#include "Player.hpp"
#include "engine/DebugRenderer.hpp"
#include "engine/Geometry.hpp"
#include "engine/Light.hpp"
#include "engine/Random.hpp"
#include "engine/Renderer.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Terrain.hpp"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <thread>

using namespace std::chrono_literals;

void FPSGame::startup() {}

void FPSGame::shutdown() {}

FPSGame::FPSGame() {
  InputManager::instance().keyPressHandlers[GLFW_KEY_ESCAPE] = [this]() {
    auto window = gRenderer.getWindow();
    int curMode = glfwGetInputMode(window, GLFW_CURSOR);
    if (curMode == GLFW_CURSOR_NORMAL) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      player->inputEnabled = false;
      drone->disableInput();
    }
  };
}

FPSGame::~FPSGame() { saveScene(); }

static void displayFPS(FPSGame::Duration dt) {
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

void sceneGUI(std::unique_ptr<GameObject> &node) {
  if (ImGui::TreeNode(node->name.c_str())) {
    if (ImGui::CollapsingHeader("Settings")) {
      node->onGUI();
    }

    for (auto &child : node->children) {
      sceneGUI(child);
    }
    ImGui::TreePop();
  }
}

void FPSGame::update(Duration dt) {
  mScene->forEach([dt](GameObject &obj) { obj.update(dt); });

  displayFPS(dt);

  ImGui::Begin("Editor");

  if (ImGui::CollapsingHeader("Settings")) {
    if (ImGui::Button(mUseColorShader ? "Use Texture shader"
                                      : "Use Color shader")) {
      mUseColorShader = !mUseColorShader;
    }

    if (ImGui::Button(mDrawCoordinateSystems ? "Hide coordinate systems"
                                             : "Draw coordinate systems")) {
      mDrawCoordinateSystems = !mDrawCoordinateSystems;
    }

    if (ImGui::Button("Reload shaders")) {
      gRenderer.reloadShaders();
      printf("shaders reloaded\n");
    }

    if (ImGui::BeginListBox("control")) {
      if (ImGui::Selectable("player")) {
        player->inputEnabled = true;
        player->resetCamera();
        drone->disableInput();
      }

      if (ImGui::Selectable("drone")) {
        drone->enableInput();
        player->inputEnabled = false;
      }
      ImGui::EndListBox();
    }

    if (ImGui::Button("Use perspective")) {
      gRenderer.usePerspective();
    }

    static glm::vec4 orthoParams{0, 1, 0, 1};

    ImGui::SliderFloat4("ortho left right bottom top", &orthoParams[0], -400.f,
                        400.f);

    if (ImGui::Button("Use orthogonal")) {
      gRenderer.useOrthogonal(orthoParams);
    }

    static float fov = gRenderer.getFov();
    if (ImGui::SliderFloat("field of view", &fov, 0.0f, 90.0f)) {
      gRenderer.setFov(fov);
    }
  }

  if (ImGui::CollapsingHeader("Entities")) {
    sceneGUI(mScene);
  }

  ImGui::End();
}

void FPSGame::render() {
  gRenderer.setView(mCamera.getViewMatrix());

  // Draw different coordinate systems
  if (mDrawCoordinateSystems) {
    ShaderHandle handle(Renderer::ShaderID::Color);

    handle.shader().setMat4("view", gRenderer.getView());
    handle.shader().setMat4("projection", gRenderer.getProjection());

    mScene->forEach(
        [this, &handle](GameObject &entity, const glm::mat4 &parentTransform) {
          auto drawCoordinates = [this, &handle]() {
            static Mesh xAxis = std::move(Geometry::makeXAxis().load());
            static Mesh yAxis = std::move(Geometry::makeYAxis().load());
            static Mesh zAxis = std::move(Geometry::makeZAxis().load());

            handle.shader().setVec3("color", Util::red);
            xAxis.draw(handle.shader(), GL_LINES);

            handle.shader().setVec3("color", Util::green);
            yAxis.draw(handle.shader(), GL_LINES);

            handle.shader().setVec3("color", Util::blue);
            zAxis.draw(handle.shader(), GL_LINES);
          };

          const auto modelBase =
              parentTransform * entity.transform.modelMatrix();

          handle.shader().setMat4("model", modelBase);

          drawCoordinates();

          if (entity.skinnedModel) {
            Transform t{modelBase *
                        entity.skinnedModel->skeleton.joints[1].finalTransform};
            t.scale = glm::vec3{1.0f};
            handle.shader().setMat4("model", t.modelMatrix());
            drawCoordinates();
          }
        },
        glm::mat4{1.0f});
  }

  // Draw scene
  {
    ShaderHandle handle(mUseColorShader ? Renderer::ShaderID::Color
                                        : Renderer::ShaderID::Model);

    handle.shader().setMat4("view", gRenderer.getView());
    handle.shader().setMat4("projection", gRenderer.getProjection());
    handle.shader().setVec3("lightPosition",
                            mScene->findChildren("light")->transform.position);
    handle.shader().setVec3("cameraPosition", mCamera.getPosition());

    mScene->render(handle.shader());
  }
}

void FPSGame::run() {
  enum class GameState { Loading, Running };

  auto state = GameState::Loading;

  while (!glfwWindowShouldClose(gRenderer.getWindow())) {
    const auto dt = mTimer.tick();

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    switch (state) {
    case GameState::Loading:
      if (gResources.isPrepared()) {
        gResources.loadAll();
        loadScene();

        mScene->forEach([](GameObject &obj) { obj.ready(); });

        for (int i = 0; i < 10; i++) {
          gDebugRenderer.addSphere(glm::vec3{2 * i, 0, 0}, 1, Util::red,
                                   (1 + i) * 1s);
        }

        state = GameState::Running;
      } else {
        break;
      }
    case GameState::Running:
      update(dt);
      gDebugRenderer.update(dt);
      break;
    }

    ImGui::Render();
    glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (state) {
    case GameState::Loading:
      // Render loading screen
      break;
    case GameState::Running:
      render();
      gDebugRenderer.render();
      break;
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(gRenderer.getWindow());
  }

  if (state == GameState::Running) {
    mScene->forEach([](GameObject &obj) { obj.shutdown(); });

    saveScene();
  }
}

void FPSGame::buildScene() {
  mScene = std::make_unique<GameObject>();
  mScene->name = "World";

  auto plr = std::make_unique<Player>(mCamera);
  plr->name = "player";
  plr->skinnedModel = ResourceManager::instance().getSkinnedModel("soldier");
  player = plr.get();
  plr->parent = mScene.get();

  auto drone = std::make_unique<Drone>(mCamera);
  drone->name = "drone";
  drone->skinnedModel = ResourceManager::instance().getSkinnedModel("soldier");
  this->drone = drone.get();
  drone->parent = mScene.get();

  auto nurse = std::make_unique<GameObject>();
  nurse->name = "nurse";
  nurse->skinnedModel = ResourceManager::instance().getSkinnedModel("nurse");
  nurse->parent = mScene.get();

  auto soldier = std::make_unique<GameObject>();
  soldier->name = "soldier";
  soldier->skinnedModel =
      ResourceManager::instance().getSkinnedModel("soldier");
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

void FPSGame::loadScene() {
  buildScene();

  // Load transformations from a file
  try {
    std::ifstream ifs(sceneFile);
    const auto json = nlohmann::json::parse(ifs);
    mScene->forEach([&json](GameObject &entity) {
      const auto &name = entity.name;
      if (json.contains(name)) {
        GameObject::deserialize(json[name], entity);
      }
    });
  } catch (const std::exception &e) {
    std::cerr << "Unable to load scene: " << e.what() << std::endl;
  }
}

void FPSGame::saveScene() {
  nlohmann::json json;
  mScene->forEach([&json](GameObject &entity) {
    const auto &name = entity.name;
    json[name] = GameObject::serialize(entity);
  });

  try {
    std::ofstream ofs{sceneFile};
    ofs << json.dump();
  } catch (const std::exception &e) {
    std::cerr << "Failed to save scene: " << e.what() << std::endl;
  }
}
