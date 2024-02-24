//
// Created by vaige on 23.2.2024.
//

#include "Texture.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>


Texture Texture::loadFromFile(std::string path, Texture::Type type)
{
    Texture tex;
    tex.type = type;
    int x, y, n;
    unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 0);

    if (data) {
        GLenum format = [n](){
            switch (n) {
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

        glGenTextures(1, &tex.id);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        stbi_image_free(data);
        throw std::runtime_error("Unable to load texture from " + path);
    }

    tex.path = std::move(path);
    return tex;
}