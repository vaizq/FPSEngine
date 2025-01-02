cmake_minimum_required(VERSION 3.27)

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

set(LIBS 
    asio::asio 
    glfw 
    imgui::imgui 
    glm::glm 
    assimp::assimp
    nlohmann_json::nlohmann_json
    OpenAL::OpenAL
    FreeALUT::alut
)

set(ENGINE_SRC
    extern/glad.c
    extern/imgui_impl_glfw.cpp
    extern/imgui_impl_opengl3.cpp
    engine/Shader.h
    engine/Mesh.cpp
    engine/Model.cpp
    engine/Util.h
    engine/Texture.cpp
    engine/Camera.cpp
    engine/Util.cpp
    engine/ResourceManager.cpp
    engine/GameObject.cpp
    engine/Transform.cpp
    engine/MyGui.cpp
    engine/InputManager.h
    engine/Shader.cpp
    engine/Light.cpp
    engine/Geometry.cpp
    engine/Terrain.cpp
    engine/Renderer.cpp
    engine/Math.cpp
    engine/RayCast.cpp
    engine/BoundingVolume.cpp
    engine/Random.cpp
)

add_library(engine ${ENGINE_SRC})
target_link_libraries(engine PUBLIC ${LIBS})
