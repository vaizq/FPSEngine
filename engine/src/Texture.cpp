//
// Created by vaige on 23.2.2024.
//

#include "Texture.hpp"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture& Texture::prepare(std::string filepath) {
    path = std::move(filepath);
    data = stbi_load(path.c_str(), &width, &height, &numComponents, 0);
    return *this;
}


Texture& Texture::load() {
    if (!data) {
        printf("WARNING: Texture %s not prepared or already load\n", path.c_str());
        return *this;
    }

    GLenum format = [this](){
        switch (numComponents) {
            case 1:
                return GL_RED;
            case 3:
                return GL_RGB;
            case 4:
                return GL_RGBA;
            default:
                return GL_RGB;
        }
    }();

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    data = nullptr;

    return *this;
}
