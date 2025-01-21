#include <iostream>
#include "FPSGame.h"
#include "engine/Renderer.hpp"
#include "engine/ResourceManager.hpp"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "engine/DebugRenderer.hpp"
#include "engine/InputManager.hpp"


int main()
{
    try {
        gRenderer.startup("FPS Game");
        gResources.startup();
        gDebugRenderer.startup();

        InputManager::registerCallbacks(gRenderer.getWindow());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(gRenderer.getWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 150");

        {
            FPSGame game;
            game.run();
        }

        gDebugRenderer.shutdown();
        gResources.shutdown();
        gRenderer.shutdown();
    } catch(const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}
