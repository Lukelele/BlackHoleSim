#include "Engine/Renderer.h"

#include <algorithm>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>

#include "Core/BlackHole.h"
#include "Core/Spacetime.h"
#include "Engine/Camera.h"
#include "Engine/Shader.h"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

// Global state for GLFW input callbacks
static Camera g_Camera(glm::vec3(0.0f), 18.0f, 0.0f, 6.0f);
static bool g_LeftMouseDown = false;
static bool g_RightMouseDown = false;
static double g_LastX = 0.0;
static double g_LastY = 0.0;
static bool g_FirstMouse = true;
static bool g_ShowUI = true;
static bool g_PauseAnimation = false;

// GLFW mouse button callback (chained via ImGui backend)
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  ImGuiIO &io = ImGui::GetIO();

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      // Only start camera orbit if not clicking on an ImGui UI window
      if (!io.WantCaptureMouse) {
        g_LeftMouseDown = true;
        glfwGetCursorPos(window, &g_LastX, &g_LastY);
      }
    } else if (action == GLFW_RELEASE) {
      g_LeftMouseDown = false;
    }
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      if (!io.WantCaptureMouse) {
        g_RightMouseDown = true;
        glfwGetCursorPos(window, &g_LastX, &g_LastY);
      }
    } else if (action == GLFW_RELEASE) {
      g_RightMouseDown = false;
    }
  }
}

// GLFW cursor position callback (chained via ImGui backend)
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  if (g_FirstMouse) {
    g_LastX = xpos;
    g_LastY = ypos;
    g_FirstMouse = false;
    return;
  }

  float xoffset = (float)(xpos - g_LastX);
  float yoffset =
      (float)(g_LastY -
              ypos); // Reversed since y-coordinates range from bottom to top
  g_LastX = xpos;
  g_LastY = ypos;

  // Only move camera if mouse was pressed outside of UI
  if (g_LeftMouseDown) {
    g_Camera.ProcessMouseMovement(xoffset, yoffset);
  } else if (g_RightMouseDown) {
    g_Camera.ProcessMousePan(xoffset, yoffset);
  }
}

// GLFW scroll callback for zoom (chained via ImGui backend)
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;

  g_Camera.ProcessMouseScroll((float)yoffset);
}

// GLFW key callback (chained via ImGui backend)
void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (action != GLFW_PRESS)
    return;

  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard)
    return;

  if (key == GLFW_KEY_H || key == GLFW_KEY_TAB) {
    g_ShowUI = !g_ShowUI;
  } else if (key == GLFW_KEY_R) {
    g_Camera.SetPreset(CameraPreset::CINEMATIC_EDGE_ON);
  } else if (key == GLFW_KEY_1) {
    g_Camera.SetPreset(CameraPreset::CINEMATIC_EDGE_ON);
  } else if (key == GLFW_KEY_2) {
    g_Camera.SetPreset(CameraPreset::POLAR_OVERHEAD);
  } else if (key == GLFW_KEY_3) {
    g_Camera.SetPreset(CameraPreset::PERSPECTIVE_45);
  } else if (key == GLFW_KEY_4) {
    g_Camera.SetPreset(CameraPreset::PHOTON_RING_CLOSEUP);
  } else if (key == GLFW_KEY_SPACE) {
    g_PauseAnimation = !g_PauseAnimation;
  } else if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main() {
  std::cout << "========================================================\n";
  std::cout << "  3D General Relativistic Black Hole Ray Tracer\n";
  std::cout << "  Einstein Null Geodesic Integration on GPU\n";
  std::cout << "========================================================\n"
            << std::endl;

  Renderer renderer(DEFAULT_WIDTH, DEFAULT_HEIGHT,
                    "General Relativity Black Hole Simulation");
  GLFWwindow *window = renderer.GetWindow();
  if (!window) {
    std::cerr << "Failed to initialize renderer window!" << std::endl;
    return -1;
  }

  // Register user GLFW callbacks first
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSetScrollCallback(window, ScrollCallback);
  glfwSetKeyCallback(window, KeyCallback);

  // Install ImGui GLFW callbacks so ImGui processes all events (hover, click,
  // drag, focus) and seamlessly chains calls into our user callbacks above.
  ImGui_ImplGlfw_InstallCallbacks(window);

  // Initialize shader
  Shader rayTracer("src/Shaders/RayTracer.vert", "src/Shaders/RayTracer.frag");
  if (!rayTracer.IsValid()) {
    std::cerr << "Fatal Error: Failed to compile ray tracer shaders!"
              << std::endl;
    return -1;
  }

  // Black Hole & Spacetime setup (Interstellar Gargantua default)
  BlackHole blackHole(1.0f, 0.75f);
  Spacetime spacetime(blackHole);

  // Accretion Disk parameters (Interstellar luminous accretion disc)
  bool enableDisk = true;
  bool lockToISCO = true;
  float diskInnerRadius = blackHole.GetISCORadius();
  float diskOuterRadius = 18.5f;
  float diskBrightness = 1.35f;
  float diskTemperature = 9500.0f; // Kelvin
  float diskScaleHeight = 0.055f;
  float diskRotationSpeed = 0.85f;
  float diskTiltPitch = 0.0f; // degrees
  float diskTiltYaw = 0.0f;   // degrees

  // Relativistic effect toggles
  bool enableDoppler = true;
  bool enableRedshift = true;

  // Ray tracing parameters
  float stepFactor = 1.0f; // 1.0 = Normal, 0.6 = Fine, 1.5 = Fast
  int maxSteps = 240;
  float starfieldBrightness = 0.8f;
  float exposure = 1.25f;

  // Cached OpenGL hardware information
  const char* glVersionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char* glRendererStr = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

  // Simulation time
  float simTime = 0.0f;

  // Main render loop
  while (renderer.IsWindowOpen()) {
    float dt = renderer.GetDeltaTime();

    // Process continuous keyboard navigation (if not typing into ImGui)
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard) {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_Camera.ProcessKeyboard(CameraDirection::FORWARD, dt);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_Camera.ProcessKeyboard(CameraDirection::BACKWARD, dt);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_Camera.ProcessKeyboard(CameraDirection::LEFT, dt);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_Camera.ProcessKeyboard(CameraDirection::RIGHT, dt);
      if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        g_Camera.ProcessKeyboard(CameraDirection::UP, dt);
      if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        g_Camera.ProcessKeyboard(CameraDirection::DOWN, dt);

      static bool s_mKeyPressed = false;
      if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!s_mKeyPressed) {
          blackHole.SetSchwarzschild(!blackHole.IsSchwarzschild());
          s_mKeyPressed = true;
        }
      } else {
        s_mKeyPressed = false;
      }
    }

    if (!g_PauseAnimation) {
      simTime += dt * diskRotationSpeed;
    }

    if (lockToISCO) {
      diskInnerRadius = blackHole.GetISCORadius();
    }

    // Calculate spin axis vector from tilt angles
    float pitchRad = glm::radians(diskTiltPitch);
    float yawRad = glm::radians(diskTiltYaw);
    glm::vec3 spinAxis(std::sin(yawRad) * std::cos(pitchRad),
                       std::cos(pitchRad), std::sin(pitchRad));
    spinAxis = glm::normalize(spinAxis);
    blackHole.SetSpinAxis(spinAxis);

    // Start frame
    renderer.StartFrame(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // 1. Render Ray Tracer into the offscreen scalable FBO
    renderer.BindRenderFBO();
    rayTracer.Bind();

    // Camera uniforms
    rayTracer.SendUniform("u_CameraPos", g_Camera.GetPosition());
    rayTracer.SendUniform("u_CameraForward", g_Camera.GetForward());
    rayTracer.SendUniform("u_CameraRight", g_Camera.GetRight());
    rayTracer.SendUniform("u_CameraUp", g_Camera.GetUp());
    rayTracer.SendUniform("u_Resolution",
                          glm::vec2((float)renderer.GetRenderWidth(),
                                    (float)renderer.GetRenderHeight()));
    rayTracer.SendUniform("u_FOV", g_Camera.GetFOV());
    rayTracer.SendUniform("u_Time", simTime);

    // Black Hole physics uniforms
    rayTracer.SendUniform("u_Mass", blackHole.GetMass());
    rayTracer.SendUniform("u_Spin", blackHole.GetSpin());
    rayTracer.SendUniform("u_SpinAxis", blackHole.GetSpinAxis());

    // Accretion Disk uniforms
    rayTracer.SendUniform("u_EnableAccretionDisk", enableDisk);
    rayTracer.SendUniform("u_DiskInnerRadius", diskInnerRadius);
    rayTracer.SendUniform("u_DiskOuterRadius", diskOuterRadius);
    rayTracer.SendUniform("u_DiskBrightness", diskBrightness);
    rayTracer.SendUniform("u_DiskTemperature", diskTemperature);
    rayTracer.SendUniform("u_DiskScaleHeight", diskScaleHeight);
    rayTracer.SendUniform("u_EnableDoppler", enableDoppler);
    rayTracer.SendUniform("u_EnableRedshift", enableRedshift);

    // Quality and visual uniforms
    rayTracer.SendUniform("u_StepFactor", stepFactor);
    rayTracer.SendUniform("u_MaxSteps", maxSteps);
    rayTracer.SendUniform("u_StarfieldBrightness", starfieldBrightness);
    rayTracer.SendUniform("u_Exposure", exposure);

    // Draw ray traced image across fullscreen quad into FBO
    renderer.RenderFullscreenQuad();

    // 2. Hardware blit from FBO to screen framebuffer
    renderer.BlitFBOToScreen();

    // 3. Render Interactive ImGui Dashboard at native resolution
    if (g_ShowUI) {
      renderer.BeginImGui();

      ImGui::SetNextWindowPos(ImVec2(18.0f, 18.0f), ImGuiCond_FirstUseEver);
      ImGui::SetNextWindowSize(ImVec2(390.0f, 620.0f), ImGuiCond_FirstUseEver);

      if (ImGui::Begin("Black Hole General Relativity Simulation", &g_ShowUI)) {
        ImGui::TextColored(ImVec4(0.4f, 0.75f, 1.0f, 1.0f),
                           "Einstein Geodesic Ray Tracer");
        ImGui::TextDisabled("Press [Tab] or [H] to hide/show UI");

        ImGui::Separator();

        // 1. Black Hole Spacetime Section
        if (ImGui::CollapsingHeader("Spacetime & Black Hole",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::TextColored(ImVec4(0.4f, 0.85f, 1.0f, 1.0f), "Metric Geometry:");
          bool isSchwarzschild = blackHole.IsSchwarzschild();
          if (ImGui::RadioButton("Schwarzschild (Static)", isSchwarzschild)) {
            blackHole.SetSchwarzschild(true);
          }
          ImGui::SameLine();
          if (ImGui::RadioButton("Kerr (Rotating)", !isSchwarzschild)) {
            blackHole.SetSchwarzschild(false);
          }

          ImGui::Spacing();

          float mass = blackHole.GetMass();
          if (ImGui::SliderFloat("Mass (M)", &mass, 0.2f, 4.0f, "%.2f")) {
            blackHole.SetMass(mass);
          }

          if (blackHole.IsSchwarzschild()) {
            ImGui::BeginDisabled();
            float zeroSpin = 0.0f;
            ImGui::SliderFloat("Kerr Spin (a)", &zeroSpin, -0.998f, 0.998f, "0.000 (Schwarzschild)");
            ImGui::EndDisabled();
            ImGui::TextDisabled("Spin is locked to 0 for Schwarzschild spacetime.");
          } else {
            float spin = blackHole.GetSpin();
            if (ImGui::SliderFloat("Kerr Spin (a)", &spin, -0.998f, 0.998f,
                                   "%.3f")) {
              blackHole.SetSpin(spin);
            }
          }

          if (blackHole.IsSchwarzschild()) {
            ImGui::TextColored(ImVec4(0.7f, 0.9f, 1.0f, 1.0f), "Relativistic Horizons (Schwarzschild r_s = 2M):");
            ImGui::BulletText("Event Horizon (r_s): %.2f M  (Exact 2M)",
                              blackHole.GetEventHorizonRadius());
            ImGui::BulletText("Photon Sphere (r_ph): %.2f M  (Exact 3M)",
                              blackHole.GetPhotonSphereRadius());
            ImGui::BulletText("ISCO Radius: %.2f M  (Exact 6M)", blackHole.GetISCORadius());
            ImGui::BulletText("Shadow Capture (b_c): %.2f M  (Exact 3*sqrt(3)*M)",
                              blackHole.GetCriticalImpactParameter());
          } else {
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.5f, 1.0f), "Relativistic Horizons (Kerr a = %.3f):", blackHole.GetSpin());
            ImGui::BulletText("Event Horizon (r_H): %.2f M",
                              blackHole.GetEventHorizonRadius());
            ImGui::BulletText("Photon Sphere (r_ph): %.2f M",
                              blackHole.GetPhotonSphereRadius());
            ImGui::BulletText("ISCO Radius: %.2f M", blackHole.GetISCORadius());
            ImGui::BulletText("Shadow Capture (b_c): %.2f M",
                              blackHole.GetCriticalImpactParameter());
          }

          ImGui::Spacing();
          ImGui::SliderFloat("Axis Pitch", &diskTiltPitch, -45.0f, 45.0f,
                             "%.1f deg");
          ImGui::SliderFloat("Axis Yaw", &diskTiltYaw, -180.0f, 180.0f,
                             "%.1f deg");
        }

        // 2. Accretion Disk Section
        if (ImGui::CollapsingHeader("Accretion Disk",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Checkbox("Enable Accretion Disk", &enableDisk);

          if (enableDisk) {
            ImGui::Checkbox("Lock Inner Edge to ISCO", &lockToISCO);
            if (!lockToISCO) {
              ImGui::SliderFloat("Inner Radius", &diskInnerRadius,
                                 blackHole.GetEventHorizonRadius(), 12.0f,
                                 "%.2f M");
            } else {
              ImGui::TextDisabled("Inner Radius: %.2f M (ISCO)",
                                  diskInnerRadius);
            }

            ImGui::SliderFloat("Outer Radius", &diskOuterRadius,
                               diskInnerRadius + 1.0f, 35.0f, "%.1f M");
            ImGui::SliderFloat("Brightness", &diskBrightness, 0.1f, 3.5f,
                               "%.2f");
            ImGui::SliderFloat("Base Temp (K)", &diskTemperature, 2500.0f,
                               20000.0f, "%.0f K");
            ImGui::SliderFloat("Scale Height (H)", &diskScaleHeight, 0.02f,
                               0.20f, "%.3f");
            ImGui::SliderFloat("Rotation Speed", &diskRotationSpeed, 0.0f, 3.0f,
                               "%.2fx");
          }
        }

        // 3. Relativistic Effects Section
        if (ImGui::CollapsingHeader("Relativistic Optics",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Checkbox("Relativistic Doppler Beaming", &enableDoppler);
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Beams light forward for gas orbiting towards "
                              "the observer,\ncreating the iconic asymmetric "
                              "Interstellar brightness.");
          }

          ImGui::Checkbox("Gravitational Redshift", &enableRedshift);
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Redshifts light climbing out of the intense "
                              "gravitational potential,\nshifting frequencies "
                              "towards red and dimming near the horizon.");
          }
        }

        // 4. Ray Tracing Quality & Optics
        if (ImGui::CollapsingHeader("Ray Tracing & Camera",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          ImGui::Text("Render Scale (Dynamic Resolution):");
          float currentScale = renderer.GetRenderScale();
          if (ImGui::SliderFloat("Scale", &currentScale, 0.35f, 1.0f,
                                 "%.2fx")) {
            renderer.SetRenderScale(currentScale);
          }
          if (ImGui::Button("50% (Fast)"))
            renderer.SetRenderScale(0.50f);
          ImGui::SameLine();
          if (ImGui::Button("65% (Balanced)"))
            renderer.SetRenderScale(0.65f);
          ImGui::SameLine();
          if (ImGui::Button("100% (Native)"))
            renderer.SetRenderScale(1.0f);

          ImGui::Spacing();
          ImGui::Text("Integration Quality Preset:");
          if (ImGui::Button("Fast")) {
            stepFactor = 1.3f;
            maxSteps = 75;
          }
          ImGui::SameLine();
          if (ImGui::Button("Balanced")) {
            stepFactor = 1.0f;
            maxSteps = 120;
          }
          ImGui::SameLine();
          if (ImGui::Button("Cinematic")) {
            stepFactor = 0.8f;
            maxSteps = 200;
          }

          ImGui::SliderFloat("Step Factor", &stepFactor, 0.5f, 2.0f, "%.2f");
          ImGui::SliderInt("Max Steps", &maxSteps, 30, 300);

          ImGui::Separator();
          float fov = g_Camera.GetFOV();
          if (ImGui::SliderFloat("Camera FOV", &fov, 25.0f, 95.0f,
                                 "%.1f deg")) {
            g_Camera.SetFOV(fov);
          }
          float dist = g_Camera.GetDistance();
          if (ImGui::SliderFloat("Distance", &dist, 3.5f, 50.0f, "%.1f M")) {
            g_Camera.SetDistance(dist);
          }
          ImGui::SliderFloat("Exposure", &exposure, 0.2f, 3.0f, "%.2f");
          ImGui::SliderFloat("Starfield Brightness", &starfieldBrightness, 0.0f,
                             3.0f, "%.2f");

          ImGui::Separator();
          ImGui::Text("Camera Presets (Keys 1-4):");
          if (ImGui::Button("1: Edge-On"))
            g_Camera.SetPreset(CameraPreset::CINEMATIC_EDGE_ON);
          ImGui::SameLine();
          if (ImGui::Button("2: Polar"))
            g_Camera.SetPreset(CameraPreset::POLAR_OVERHEAD);
          if (ImGui::Button("3: 45° Angle"))
            g_Camera.SetPreset(CameraPreset::PERSPECTIVE_45);
          ImGui::SameLine();
          if (ImGui::Button("4: Photon Ring"))
            g_Camera.SetPreset(CameraPreset::PHOTON_RING_CLOSEUP);
        }

        // 5. Performance & Diagnostics
        if (ImGui::CollapsingHeader("Performance & Info",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
          float fps = renderer.GetFPS();
          ImVec4 fpsColor = (fps >= 45.0f)   ? ImVec4(0.3f, 0.95f, 0.4f, 1.0f)
                            : (fps >= 25.0f) ? ImVec4(0.95f, 0.85f, 0.2f, 1.0f)
                                             : ImVec4(0.95f, 0.3f, 0.2f, 1.0f);
          ImGui::TextColored(fpsColor, "FPS: %.1f (%.1f ms)", fps,
                             1000.0f / std::max(1.0f, fps));
          ImGui::Text("Ray Trace Res: %dx%d (%.0f%%)",
                      renderer.GetRenderWidth(), renderer.GetRenderHeight(),
                      renderer.GetRenderScale() * 100.0f);
          ImGui::Text("UI/Display Res: %dx%d (Native)",
                      renderer.GetFramebufferWidth(),
                      renderer.GetFramebufferHeight());
          ImGui::Text("OpenGL: %s", glVersionStr ? glVersionStr : "Unknown");
          ImGui::Text("GPU: %s", glRendererStr ? glRendererStr : "Unknown");
          ImGui::Separator();
          ImGui::Text("Controls:");
          ImGui::BulletText("Left Drag: Orbit Camera");
          ImGui::BulletText("Right Drag: Pan Target");
          ImGui::BulletText("Scroll: Zoom");
          ImGui::BulletText("WASD / QE: Fly");
          ImGui::BulletText("M: Toggle Schwarzschild / Kerr Metric");
          ImGui::BulletText("Space: Pause Disk Animation");
        }
      }
      ImGui::End();

      renderer.EndImGui();
    }

    // End frame and swap buffers
    renderer.EndFrame();
  }

  std::cout << "Application exited successfully." << std::endl;
  return 0;
}
