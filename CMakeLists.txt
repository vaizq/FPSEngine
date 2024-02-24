cmake_minimum_required(VERSION 3.27)
project(FPSFromScratch)

set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DSRC_DIR="${SRC_DIR}")

find_package(glfw3 CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(assimp CONFIG REQUIRED)
add_executable(FPSFromScratch main.cpp
        Game.cpp
        extern/glad.c
        extern/imgui_impl_glfw.cpp
        extern/imgui_impl_opengl3.cpp
        FPSGame.cpp
        Shader.h
        Mesh.cpp
        Model.cpp
        Util.h
        Texture.cpp
        Camera.cpp)

target_include_directories(FPSFromScratch PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/imgui ${Stb_INCLUDE_DIR})

target_link_libraries(FPSFromScratch PRIVATE asio::asio glfw imgui::imgui glm::glm assimp::assimp)
