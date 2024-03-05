//
// Created by vaige on 23.2.2024.
//

#ifndef FPSFROMSCRATCH_TEXTURE_H
#define FPSFROMSCRATCH_TEXTURE_H

#include <string>
#include <memory>

struct Texture
{
    static Texture loadFromFile(std::string path, bool gamma = false);

    unsigned id{};
    std::string type;
    std::string path;
};





#endif //FPSFROMSCRATCH_TEXTURE_H
