#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0x91B9
#endif

class Shader {
public:
    Shader();
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
    Shader(const char* computeShaderPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void Bind() const;
    void UnBind() const;

    bool AddShadersByFilepath(const char* vertexShaderPath, const char* fragmentShaderPath);
    bool AddComputeShaderByFilepath(const char* computeShaderPath);

    void SendUniform(const char* uniformName, float x);
    void SendUniform(const char* uniformName, float x, float y);
    void SendUniform(const char* uniformName, float x, float y, float z);
    void SendUniform(const char* uniformName, float x, float y, float z, float w);
    void SendUniform(const char* uniformName, const glm::vec2& v);
    void SendUniform(const char* uniformName, const glm::vec3& v);
    void SendUniform(const char* uniformName, const glm::vec4& v);

    void SendUniform(const char* uniformName, int x);
    void SendUniform(const char* uniformName, int x, int y);
    void SendUniform(const char* uniformName, int x, int y, int z);
    void SendUniform(const char* uniformName, int x, int y, int z, int w);
    void SendUniform(const char* uniformName, bool b);

    void SendUniform(const char* uniformName, const glm::mat3& matrix3);
    void SendUniform(const char* uniformName, const glm::mat4& matrix4);

    GLuint GetProgramID() const { return programID; }
    bool IsValid() const { return programID != 0; }

private:
    GLuint programID = 0;
    GLuint compileShader(const char* shaderSource, GLenum shaderType);
    std::string loadShaderSource(const char* filepath);
    GLint getUniformLocation(const char* uniformName);

    std::unordered_map<std::string, GLint> uniformLocations;
};
