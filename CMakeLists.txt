cmake_minimum_required(VERSION 3.27)
project(FPSFromScratch)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_executable(FPSFromScratch main.cpp
        Game.cpp
        extern/glad.c)

target_include_directories(FPSFromScratch PRIVATE include ${Stb_INCLUDE_DIR})

target_link_libraries(FPSFromScratch PRIVATE asio::asio glfw imgui::imgui glm::glm)