#include "Shader.h"

Shader::Shader()
{
	programID = glCreateProgram();
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
	programID = glCreateProgram();
	AddShadersByFilepath(vertexShaderPath, fragmentShaderPath);
	Bind();
}

Shader::~Shader()
{
	glDeleteProgram(programID);
}

void Shader::Bind() {
	glUseProgram(programID);
}

void Shader::UnBind() {
	glUseProgram(0);
}

void Shader::AddShadersBySource(const char* vertexShaderSource, const char* fragmentShaderSource) {
	GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint length;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(programID, length, &length, message);
        cout << "Failed to link program!" << endl;
        cout << message << endl;
    }

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::AddShadersByFilepath(const char* vertexShaderPath, const char* fragmentShaderPath) {
	//const char* vertexShaderSource = loadShaderSource(vertexShaderPath).c_str();
	//const char* fragmentShaderSource = loadShaderSource(fragmentShaderPath).c_str();    do not convert, pass right into function instead

	GLuint vertexShader = compileShader(loadShaderSource(vertexShaderPath).c_str(), GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(loadShaderSource(fragmentShaderPath).c_str(), GL_FRAGMENT_SHADER);

	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint length;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(programID, length, &length, message);
        cout << "Failed to link program!" << endl;
        cout << message << endl;
    }

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


GLuint Shader::compileShader(const char* shaderSource, GLenum shaderType) {
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &shaderSource, 0);
	glCompileShader(shaderID);

	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shaderID, length, &length, message);
        std::cout << "Failed to compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(shaderID);
        return 0;
    }

	return shaderID;
}

string Shader::loadShaderSource(const char* filepath) {
	ifstream file(filepath);
	string source((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));

	return source;
}



void Shader::SendUniform(const char* uniformName, float x) 
{
	glUniform1f(getUniformLocation(programID, uniformName), x);
}

void Shader::SendUniform(const char* uniformName, float x, float y)
{
	glUniform2f(getUniformLocation(programID, uniformName), x, y);
}

void Shader::SendUniform(const char* uniformName, float x, float y, float z)
{
	glUniform3f(getUniformLocation(programID, uniformName), x, y, z);
}

void Shader::SendUniform(const char* uniformName, float x, float y, float z, float w)
{
	glUniform4f(getUniformLocation(programID, uniformName), x, y, z, w);
}

void Shader::SendUniform(const char* uniformName, int x)
{
	glUniform1i(getUniformLocation(programID, uniformName), x);
}

void Shader::SendUniform(const char* uniformName, int x, int y)
{
	glUniform2i(getUniformLocation(programID, uniformName), x, y);
}

void Shader::SendUniform(const char* uniformName, int x, int y, int z)
{
	glUniform3i(getUniformLocation(programID, uniformName), x, y, z);
}

void Shader::SendUniform(const char* uniformName, int x, int y, int z, int w)
{
	glUniform4i(getUniformLocation(programID, uniformName), x, y, z, w);
}

void Shader::SendUniform(const char* uniformName, mat4 &matrix4) {
	glUniformMatrix4fv(getUniformLocation(programID, uniformName), 1, false, &matrix4[0][0]);
}


GLuint Shader::getUniformLocation(GLuint programID, const char* uniformName) {
	if (uniformLocations[uniformName] == 0) {
		uniformLocations[uniformName] = glGetUniformLocation(programID, uniformName);
		if (uniformLocations[uniformName] == -1) {
			cout << "Uniform: " << uniformName << " Not Found." << endl;
		}
	}

	return uniformLocations[uniformName];
}