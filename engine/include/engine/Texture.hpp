//
// Created by vaige on 23.2.2024.
//

#ifndef FPSFROMSCRATCH_TEXTURE_H
#define FPSFROMSCRATCH_TEXTURE_H

#include <string>

class Texture
{
public:
    Texture& prepare(std::string path);
    Texture& load();

    unsigned id{};
    std::string type{"texture_diffuse"};
    std::string path;
private:
    int width, height, numComponents;
    unsigned char* data;
};

#endif //FPSFROMSCRATCH_TEXTURE_H
