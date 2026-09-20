#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Renderer::Renderer(int width, int height, const char* title)
    : m_width(width), m_height(height), m_renderScale(0.55f)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable V-Sync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        return;
    }

    glfwGetFramebufferSize(m_window, &m_fbWidth, &m_fbHeight);
    glViewport(0, 0, m_fbWidth, m_fbHeight);

    // Check OpenGL version for compute shader capability
    int major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    m_supportsCompute = (major > 4 || (major == 4 && minor >= 3));

    SetupFullscreenQuad();
    SetupFBO();
    InitImGui();

    m_lastFrameTime = (float)glfwGetTime();
}

Renderer::~Renderer() {
    ShutdownImGui();
    CleanupFBO();

    if (m_quadVAO) {
        glDeleteVertexArrays(1, &m_quadVAO);
        glDeleteBuffers(1, &m_quadVBO);
        m_quadVAO = 0;
        m_quadVBO = 0;
    }

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Renderer::SetupFBO() {
    CleanupFBO();

    m_renderWidth = std::max(64, (int)(m_fbWidth * m_renderScale));
    m_renderHeight = std::max(64, (int)(m_fbHeight * m_renderScale));

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_renderWidth, m_renderHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer incomplete! Status: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CleanupFBO() {
    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    if (m_fboTexture) {
        glDeleteTextures(1, &m_fboTexture);
        m_fboTexture = 0;
    }
}

void Renderer::SetRenderScale(float scale) {
    scale = std::clamp(scale, 0.25f, 1.0f);
    if (std::abs(m_renderScale - scale) > 0.01f) {
        m_renderScale = scale;
        SetupFBO();
    }
}

void Renderer::BindRenderFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_renderWidth, m_renderHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::BlitFBOToScreen() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_renderWidth, m_renderHeight,
                      0, 0, m_fbWidth, m_fbHeight,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_fbWidth, m_fbHeight);
}

void Renderer::SetupFullscreenQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Renderer::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowPadding = ImVec2(14.0f, 14.0f);
    style.FramePadding = ImVec2(8.0f, 5.0f);
    style.ItemSpacing = ImVec2(8.0f, 8.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg]             = ImVec4(0.07f, 0.08f, 0.11f, 0.90f);
    colors[ImGuiCol_Header]               = ImVec4(0.18f, 0.22f, 0.32f, 0.70f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(0.26f, 0.35f, 0.52f, 0.80f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.35f, 0.48f, 0.70f, 0.90f);
    colors[ImGuiCol_Button]               = ImVec4(0.18f, 0.24f, 0.38f, 0.75f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.28f, 0.38f, 0.60f, 0.85f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.38f, 0.50f, 0.78f, 0.95f);
    colors[ImGuiCol_FrameBg]              = ImVec4(0.12f, 0.14f, 0.20f, 0.80f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.18f, 0.22f, 0.30f, 0.80f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.24f, 0.30f, 0.42f, 0.80f);
    colors[ImGuiCol_SliderGrab]           = ImVec4(0.35f, 0.58f, 0.92f, 0.90f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_CheckMark]            = ImVec4(0.40f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_TitleBg]              = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.10f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_Border]               = ImVec4(0.22f, 0.26f, 0.36f, 0.50f);

    ImGui_ImplGlfw_InitForOpenGL(m_window, false);
    ImGui_ImplOpenGL3_Init("#version 410");
    m_imguiInitialized = true;
}

void Renderer::ShutdownImGui() {
    if (m_imguiInitialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        m_imguiInitialized = false;
    }
}

void Renderer::UpdateTiming() {
    float current = (float)glfwGetTime();
    m_deltaTime = current - m_lastFrameTime;
    m_lastFrameTime = current;
    m_totalTime = current;

    m_frameCount++;
    m_fpsTimer += m_deltaTime;
    if (m_fpsTimer >= 0.4f) {
        m_fps = (float)m_frameCount / m_fpsTimer;
        m_frameCount = 0;
        m_fpsTimer = 0.0f;
    }
}

void Renderer::StartFrame(glm::vec4 clearColor) {
    if (!m_window) return;

    UpdateTiming();

    int prevFbW = m_fbWidth;
    int prevFbH = m_fbHeight;
    glfwGetFramebufferSize(m_window, &m_fbWidth, &m_fbHeight);
    glfwGetWindowSize(m_window, &m_width, &m_height);

    if (m_fbWidth != prevFbW || m_fbHeight != prevFbH || !m_fbo) {
        SetupFBO();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_fbWidth, m_fbHeight);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::BeginImGui() {
    if (!m_imguiInitialized) return;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::EndImGui() {
    if (!m_imguiInitialized) return;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_fbWidth, m_fbHeight);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::RenderFullscreenQuad() {
    if (!m_quadVAO) return;
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::EndFrame() {
    if (!m_window) return;
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Renderer::Draw(Shader& shader, Object& object) {
    shader.Bind();
    object.GetVertexArray().Bind();
    glDrawArrays(object.GetMesh()->DrawMode, 0, object.GetMesh()->VertexCount);
}
