//
// Created by vaige on 23.2.2024.
//

#ifndef FPSFROMSCRATCH_TEXTURE_H
#define FPSFROMSCRATCH_TEXTURE_H

#include <string>

struct Texture
{
    enum class Type
    {
        Diffuse,
        Specular
    };

    static Texture loadFromFile(std::string path, Texture::Type type = Texture::Type::Diffuse);

    unsigned id{};
    Type type{};
    std::string path;
};


#endif //FPSFROMSCRATCH_TEXTURE_H
