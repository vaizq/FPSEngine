//
// Created by vaige on 20.2.2024.
//

#ifndef FPSFROMSCRATCH_UTIL_H
#define FPSFROMSCRATCH_UTIL_H


#include <string>

namespace Util
{

#ifndef SRC_DIR
#define SRC_DIR "."
#endif

    static constexpr std::string_view assetsDir = SRC_DIR "/assets";
    static constexpr std::string_view shadersDir = SRC_DIR "/shaders";

    static std::string getShaderPath(const std::string& shaderName) {
        return std::string(shadersDir) + '/' + shaderName;
    }

    static std::string getAssetPath(const std::string& assetName) {
        return std::string(assetsDir) + '/' + assetName;
    }
}

#endif //FPSFROMSCRATCH_UTIL_H
