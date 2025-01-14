//
// Created by vaige on 25.2.2024.
//

#include "Util.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <stdexcept>
#include <nlohmann/json.hpp>

#ifndef SRC_DIR
#define SRC_DIR "."
#endif

static constexpr std::string_view assetsDir = SRC_DIR "/assets";
static constexpr std::string_view shadersDir = SRC_DIR "/shaders";


std::string Util::getShaderPath(const std::string& shaderName) {
    return std::string(shadersDir) + '/' + shaderName;
}

std::string Util::getAssetPath(const std::string& assetName) {
    return std::string(assetsDir) + '/' + assetName;
}

std::string Util::extractFilename(const std::string& filepath)
{
    auto filenameBegin = [&filepath]() {
        auto result = filepath.find_last_of('/');
        if (result == std::string::npos) {
            return 0UL;
        }
        else {
            return result + 1;
        }
    }();

    auto filenameEnd = [&filepath, filenameBegin]() {
        return filenameBegin + filepath.substr(filenameBegin).find_last_of('.');
    }();

    return filepath.substr(filenameBegin, filenameEnd - filenameBegin);
}

nlohmann::json Util::serialize(const ::glm::vec3& v)
{
    return nlohmann::json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
}

glm::vec3 Util::glm::vec3::deserialize(const nlohmann::json& j)
{
    return ::glm::vec3{j["x"], j["y"], j["z"]};
}


