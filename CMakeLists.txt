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
        src/FPSGame.cpp
        src/Shader.h
        src/Mesh.cpp
        src/Model.cpp
        src/Util.h
        src/Texture.cpp
        src/Camera.cpp
        src/Util.cpp
        src/ResourceManager.cpp
        src/Entity.cpp
        src/Transform.cpp
        src/MyGui.cpp)

target_include_directories(FPSFromScratch PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/imgui ${Stb_INCLUDE_DIR})
target_link_libraries(FPSFromScratch PRIVATE asio::asio glfw imgui::imgui glm::glm assimp::assimp)
target_link_libraries(FPSFromScratch PRIVATE nlohmann_json::nlohmann_json)


find_package(Catch2 CONFIG REQUIRED)

add_executable(Tests
        tests/test_main.cpp)

target_link_libraries(Tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
