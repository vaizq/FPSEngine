cmake_minimum_required(VERSION 3.27)
project(FPSFromScratch)

set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DSRC_DIR="${SRC_DIR}")

find_package(nlohmann_json CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(FPSFromScratch src/main.cpp
        extern/glad.c
        extern/imgui_impl_glfw.cpp
        extern/imgui_impl_opengl3.cpp
        src/game/FPSGame.cpp
        src/core/Shader.h
        src/core/Mesh.cpp
        src/core/Model.cpp
        src/core/Util.h
        src/core/Texture.cpp
        src/core/Camera.cpp
        src/core/Util.cpp
        src/core/ResourceManager.cpp
        src/core/GameObject.cpp
        src/core/Transform.cpp
        src/core/MyGui.cpp
        src/core/InputManager.h)

target_include_directories(FPSFromScratch PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/imgui ${Stb_INCLUDE_DIR})
target_link_libraries(FPSFromScratch PRIVATE asio::asio glfw imgui::imgui glm::glm assimp::assimp)
target_link_libraries(FPSFromScratch PRIVATE nlohmann_json::nlohmann_json)


find_package(Catch2 CONFIG REQUIRED)

add_executable(Tests
        tests/test_main.cpp)

target_link_libraries(Tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
