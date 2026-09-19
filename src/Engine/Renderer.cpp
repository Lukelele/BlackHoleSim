#include "Renderer.h"


Renderer::Renderer(int width, int height)
    : m_window(nullptr)
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
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        return;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
}

Renderer::~Renderer()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Renderer::StartFrame(glm::vec4 clearColor)
{
    if (!m_window) return;
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame()
{
    if (!m_window) return;
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Renderer::Draw(Shader &shader, VertexArray &va, GLsizei vertexCount, GLenum drawMode)
{
    shader.Bind();
    va.Bind();
    glDrawArrays(drawMode, 0, vertexCount);
}

void Renderer::Draw(Shader &shader, Object &object)
{
    shader.Bind();
    object.GetVertexArray().Bind();
    glDrawArrays(object.GetMesh()->DrawMode, 0, object.GetMesh()->VertexCount);
}
