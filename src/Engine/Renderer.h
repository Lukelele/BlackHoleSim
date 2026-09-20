#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Mesh.h"
#include "Object.h"

class Renderer {
public:
    Renderer(int width, int height, const char* title = "General Relativity Black Hole Ray Tracer");
    ~Renderer();

    void StartFrame(glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    void EndFrame();

    // Offscreen render target management for dynamic resolution scaling
    void BindRenderFBO();
    void BlitFBOToScreen();

    // Render fullscreen quad for ray tracer
    void RenderFullscreenQuad();

    // ImGui frame handling
    void BeginImGui();
    void EndImGui();

    // Window and display properties
    GLFWwindow* GetWindow() const { return m_window; }
    bool IsWindowOpen() const { return m_window && !glfwWindowShouldClose(m_window); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetFramebufferWidth() const { return m_fbWidth; }
    int GetFramebufferHeight() const { return m_fbHeight; }
    float GetAspectRatio() const { return (float)m_renderWidth / (float)(m_renderHeight > 0 ? m_renderHeight : 1); }

    // Render scale & resolution
    float GetRenderScale() const { return m_renderScale; }
    void SetRenderScale(float scale);
    int GetRenderWidth() const { return m_renderWidth; }
    int GetRenderHeight() const { return m_renderHeight; }

    // Timing and performance
    float GetDeltaTime() const { return m_deltaTime; }
    float GetTotalTime() const { return m_totalTime; }
    float GetFPS() const { return m_fps; }

    // Compute shader support check
    bool SupportsComputeShaders() const { return m_supportsCompute; }

    // Legacy object draw
    void Draw(Shader& shader, Object& object);

private:
    void SetupFullscreenQuad();
    void SetupFBO();
    void CleanupFBO();
    void InitImGui();
    void ShutdownImGui();
    void UpdateTiming();

    GLFWwindow* m_window = nullptr;
    int m_width = 1280;
    int m_height = 720;
    int m_fbWidth = 1280;
    int m_fbHeight = 720;

    // Dynamic resolution scaling (0.35 to 1.0)
    float m_renderScale = 0.55f;
    int m_renderWidth = 704;
    int m_renderHeight = 396;

    // FBO handles
    GLuint m_fbo = 0;
    GLuint m_fboTexture = 0;

    // Fullscreen quad GL objects
    GLuint m_quadVAO = 0;
    GLuint m_quadVBO = 0;

    // Timing
    float m_lastFrameTime = 0.0f;
    float m_deltaTime = 0.016f;
    float m_totalTime = 0.0f;
    float m_fps = 60.0f;
    int m_frameCount = 0;
    float m_fpsTimer = 0.0f;

    bool m_imguiInitialized = false;
    bool m_supportsCompute = false;
};
