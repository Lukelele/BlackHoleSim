#include "Shader.h"
#include <vector>

Shader::Shader() {
    programID = glCreateProgram();
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    programID = glCreateProgram();
    AddShadersByFilepath(vertexShaderPath, fragmentShaderPath);
}

Shader::Shader(const char* computeShaderPath) {
    programID = glCreateProgram();
    AddComputeShaderByFilepath(computeShaderPath);
}

Shader::~Shader() {
    if (programID) {
        glDeleteProgram(programID);
        programID = 0;
    }
}

Shader::Shader(Shader&& other) noexcept
    : programID(other.programID), uniformLocations(std::move(other.uniformLocations))
{
    other.programID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (programID) glDeleteProgram(programID);
        programID = other.programID;
        uniformLocations = std::move(other.uniformLocations);
        other.programID = 0;
    }
    return *this;
}

void Shader::Bind() const {
    if (programID) glUseProgram(programID);
}

void Shader::UnBind() const {
    glUseProgram(0);
}

bool Shader::AddShadersByFilepath(const char* vertexShaderPath, const char* fragmentShaderPath) {
    std::string vertSource = loadShaderSource(vertexShaderPath);
    std::string fragSource = loadShaderSource(fragmentShaderPath);

    if (vertSource.empty() || fragSource.empty()) return false;

    GLuint vertexShader = compileShader(vertSource.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragSource.c_str(), GL_FRAGMENT_SHADER);

    if (!vertexShader || !fragmentShader) {
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        return false;
    }

    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetProgramInfoLog(programID, length, &length, message.data());
        std::cerr << "Failed to link shader program! Log:\n" << message.data() << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

bool Shader::AddComputeShaderByFilepath(const char* computeShaderPath) {
    std::string compSource = loadShaderSource(computeShaderPath);
    if (compSource.empty()) return false;

    GLuint computeShader = compileShader(compSource.c_str(), GL_COMPUTE_SHADER);
    if (!computeShader) return false;

    glAttachShader(programID, computeShader);
    glLinkProgram(programID);

    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetProgramInfoLog(programID, length, &length, message.data());
        std::cerr << "Failed to link compute shader program! Log:\n" << message.data() << std::endl;
        glDeleteShader(computeShader);
        return false;
    }

    glDeleteShader(computeShader);
    return true;
}

GLuint Shader::compileShader(const char* shaderSource, GLenum shaderType) {
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderSource, nullptr);
    glCompileShader(shaderID);

    int result;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(shaderID, length, &length, message.data());

        const char* typeStr = "unknown";
        if (shaderType == GL_VERTEX_SHADER) typeStr = "vertex";
        else if (shaderType == GL_FRAGMENT_SHADER) typeStr = "fragment";
        else if (shaderType == GL_COMPUTE_SHADER) typeStr = "compute";

        std::cerr << "Failed to compile " << typeStr << " shader! Log:\n" << message.data() << std::endl;
        glDeleteShader(shaderID);
        return 0;
    }

    return shaderID;
}

std::string Shader::loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

GLint Shader::getUniformLocation(const char* uniformName) {
    auto it = uniformLocations.find(uniformName);
    if (it != uniformLocations.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(programID, uniformName);
    uniformLocations[uniformName] = location;
    return location;
}

void Shader::SendUniform(const char* uniformName, float x) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform1f(loc, x);
}

void Shader::SendUniform(const char* uniformName, float x, float y) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform2f(loc, x, y);
}

void Shader::SendUniform(const char* uniformName, float x, float y, float z) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform3f(loc, x, y, z);
}

void Shader::SendUniform(const char* uniformName, float x, float y, float z, float w) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform4f(loc, x, y, z, w);
}

void Shader::SendUniform(const char* uniformName, const glm::vec2& v) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform2f(loc, v.x, v.y);
}

void Shader::SendUniform(const char* uniformName, const glm::vec3& v) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform3f(loc, v.x, v.y, v.z);
}

void Shader::SendUniform(const char* uniformName, const glm::vec4& v) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void Shader::SendUniform(const char* uniformName, int x) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform1i(loc, x);
}

void Shader::SendUniform(const char* uniformName, int x, int y) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform2i(loc, x, y);
}

void Shader::SendUniform(const char* uniformName, int x, int y, int z) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform3i(loc, x, y, z);
}

void Shader::SendUniform(const char* uniformName, int x, int y, int z, int w) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform4i(loc, x, y, z, w);
}

void Shader::SendUniform(const char* uniformName, bool b) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform1i(loc, b ? 1 : 0);
}

void Shader::SendUniform(const char* uniformName, const glm::mat3& matrix3) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniformMatrix3fv(loc, 1, GL_FALSE, &matrix3[0][0]);
}

void Shader::SendUniform(const char* uniformName, const glm::mat4& matrix4) {
    GLint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, &matrix4[0][0]);
}