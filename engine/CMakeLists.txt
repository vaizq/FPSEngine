cmake_minimum_required(VERSION 3.27)

add_library(Engine STATIC)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
#find_package(assimp CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(FreeALUT CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)


set(LIBS 
    asio::asio 
    glfw 
    imgui::imgui 
    glm::glm 
    assimp
    nlohmann_json::nlohmann_json
    OpenAL::OpenAL
    FreeALUT::alut
    Catch2::Catch2WithMain
)

set(ENGINE_SRC
    extern/glad.c
    extern/imgui_impl_glfw.cpp
    extern/imgui_impl_opengl3.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/Texture.cpp
    src/Camera.cpp
    src/Util.cpp
    src/ResourceManager.cpp
    src/GameObject.cpp
    src/Transform.cpp
    src/MyGui.cpp
    src/Shader.cpp
    src/Light.cpp
    src/Geometry.cpp
    src/Terrain.cpp
    src/RayCast.cpp
    src/BoundingVolume.cpp
    src/Random.cpp
    src/Renderer.cpp
    src/Animation.cpp
)

add_subdirectory(extern/assimp)

target_sources(Engine PRIVATE ${ENGINE_SRC})

target_include_directories(Engine
    PUBLIC
        include
    PRIVATE
        include/engine
        include/imgui
        src
)

target_link_libraries(Engine PUBLIC ${LIBS})
