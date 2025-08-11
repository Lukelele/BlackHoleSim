#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <alloca.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::istreambuf_iterator;
using std::unordered_map;
using glm::vec3;
using glm::mat4;


class Shader
{
public:
	Shader();
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	~Shader();

	void Bind();
	void UnBind();

	void AddShadersBySource(const char* vertexShaderSource, const char* fragmentShaderSource);
	void AddShadersByFilepath(const char* vertexShaderPath, const char* fragmentShaderPath);

	void SendUniform(const char* uniformName, float x);
	void SendUniform(const char* uniformName, float x, float y);
	void SendUniform(const char* uniformName, float x, float y, float z);
	void SendUniform(const char* uniformName, float x, float y, float z, float w);
	void SendUniform(const char* uniformName, int x);
	void SendUniform(const char* uniformName, int x, int y);
	void SendUniform(const char* uniformName, int x, int y, int z);
	void SendUniform(const char* uniformName, int x, int y, int z, int w);
	void SendUniform(const char* uniformName, mat4 &matrix4);
private:
	GLuint programID;
	GLuint compileShader(const char* shaderSource, GLenum shaderType);
	string loadShaderSource(const char* filepath);
	GLuint getUniformLocation(GLuint programID, const char* uniformName);

	unordered_map<string, GLint> uniformLocations;
};
