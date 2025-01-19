//
// Created by vaige on 19.2.2024.
//

#ifndef FPSFROMSCRATCH_SHADER_H
#define FPSFROMSCRATCH_SHADER_H

#include "glad/glad.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <optional>

class Shader
{
    struct UniformMat4 {
        GLint location;
        glm::mat4 value;
    };

public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    // activate the shader
    // ------------------------------------------------------------------------
    void use();

    void reload();

    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setUInt(const std::string &name, unsigned int value);
    void setFloat(const std::string &name, float value);
    void setVec2(const std::string &name, const glm::vec2 &value);
    void setVec2(const std::string &name, float x, float y);
    void setVec3(const std::string &name, const glm::vec3 &value);
    void setVec3(const std::string &name, float x, float y, float z);
    void setVec4(const std::string &name, const glm::vec4 &value);
    void setVec4(const std::string &name, float x, float y, float z, float w);
    void setMat2(const std::string &name, const glm::mat2 &mat);
    void setMat3(const std::string &name, const glm::mat3 &mat);
    void setMat4(const std::string &name, const glm::mat4 &mat);
    void setMat4(const std::string &name, const glm::mat4 &mats, size_t len);

private:
    GLuint loadShader(const char* vertexFilePath, const char* fragmentFilePath, const char* geometryFilePath = nullptr);
    void checkCompileErrors(GLuint shader, std::string type);
    GLint getUniformLocation(const std::string& name);

    std::unordered_map<std::string, GLint> mUniformLocations;
    unsigned int mID{0};
    std::string mVertexPath;
    std::string mFragmentPath;
    std::string mGeometryPath;
};

#endif //FPSFROMSCRATCH_SHADER_H
