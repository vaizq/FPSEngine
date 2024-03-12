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
find_package(OpenAL CONFIG REQUIRED)
find_package(FreeALUT CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

set(SRC_FILES
    extern/glad.c
    extern/imgui_impl_glfw.cpp
    extern/imgui_impl_opengl3.cpp
    src/game/FPSGame.cpp
        src/engine/Shader.h
        src/engine/Mesh.cpp
        src/engine/Model.cpp
        src/engine/Util.h
        src/engine/Texture.cpp
        src/engine/Camera.cpp
        src/engine/Util.cpp
        src/engine/ResourceManager.cpp
        src/engine/GameObject.cpp
        src/engine/Transform.cpp
        src/engine/MyGui.cpp
        src/engine/InputManager.h
        src/engine/Shader.cpp
        src/engine/Light.cpp
        src/engine/Geometry.cpp
)

add_executable(FPSFromScratch src/main.cpp ${SRC_FILES})

target_include_directories(FPSFromScratch PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/imgui ${Stb_INCLUDE_DIR})
target_link_libraries(FPSFromScratch PRIVATE asio::asio glfw imgui::imgui glm::glm assimp::assimp)
target_link_libraries(FPSFromScratch PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(FPSFromScratch PRIVATE OpenAL::OpenAL)
target_link_libraries(FPSFromScratch PRIVATE FreeALUT::alut)

add_executable(Tests tests/test_main.cpp ${SRC_FILES})

target_link_libraries(Tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_include_directories(Tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/imgui ${Stb_INCLUDE_DIR})
target_link_libraries(Tests PRIVATE asio::asio glfw imgui::imgui glm::glm assimp::assimp)
target_link_libraries(Tests PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(Tests PRIVATE OpenAL::OpenAL)
target_link_libraries(Tests PRIVATE FreeALUT::alut)