#include "Renderer.h"


Renderer::Renderer(int width, int height)
{
    if (!glfwInit()) {
        std::cout << "GLFW not initialised." << std::endl;
        return;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac

    m_window = glfwCreateWindow(width, height, "Black Hole", NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        return;
    }

    // Make the window's context current
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return;
    }
}

Renderer::~Renderer()
{
    glfwTerminate();
}

void Renderer::StartFrame(glm::vec4 clearColor)
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Renderer::Draw(Shader &shader, VertexArray &va, GLenum drawMode)
{
    shader.Bind();
    va.Bind();
    glDrawArrays(drawMode, 0, va.GetVertexCount());
}
