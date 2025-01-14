//
// Created by vaige on 12.3.2024.
//

#include "Shader.hpp"


Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath, const char* geometryFilePath)
: mVertexPath(vertexFilePath), mFragmentPath(fragmentFilePath), mGeometryPath(geometryFilePath == nullptr ? "" : geometryFilePath)
{
    mID = loadShader(vertexFilePath, fragmentFilePath, geometryFilePath);
}
// activate the shader
// ------------------------------------------------------------------------
void Shader::use()
{
    glUseProgram(mID);
}

void Shader::reload()
{
    glDeleteProgram(mID);
    mID = loadShader(mVertexPath.c_str(), mFragmentPath.c_str(), mGeometryPath == "" ? nullptr : mGeometryPath.c_str());
}

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value)
{
    glUniform1i(getUniformLocation(name), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUInt(const std::string &name, unsigned int value)
{
    glUniform1ui(getUniformLocation(name), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y)
{
    glUniform2f(getUniformLocation(name), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z)
{
    glUniform3f(getUniformLocation(name), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value)
{
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(getUniformLocation(name), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat)
{
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4& mats, size_t len)
{
    glUniformMatrix4fv(getUniformLocation(name), len, GL_FALSE, &mats[0][0]);
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

GLint Shader::getUniformLocation(const std::string &name)
{
    if (!mUniformLocations.contains(name)) {
        mUniformLocations.emplace(name, glGetUniformLocation(mID, name.c_str()));
    }
    return mUniformLocations[name];
}

GLuint Shader::loadShader(const char *vertexFilePath, const char *fragmentFilePath, const char *geometryFilePath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexFilePath);
        fShaderFile.open(fragmentFilePath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if(geometryFilePath != nullptr)
        {
            gShaderFile.open(geometryFilePath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if(geometryFilePath != nullptr)
    {
        const char * gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    GLuint id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    if(geometryFilePath != nullptr)
        glAttachShader(id, geometry);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(geometryFilePath != nullptr)
        glDeleteShader(geometry);

    return id;
}
