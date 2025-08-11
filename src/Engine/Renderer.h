#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "VertexArray.h"
#include "Shader.h"


class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void StartFrame(glm::vec4 clearColor = glm::vec4(0, 0, 0, 1));
    void EndFrame();
    void Draw(VertexArray &va, Shader &shader);


    GLFWwindow* GetWindow() const { return m_window; }
    void SetWindow(GLFWwindow* window) { m_window = window; }
    bool IsWindowOpen() const { return !glfwWindowShouldClose(m_window); }

private:
    GLFWwindow* m_window;
};
