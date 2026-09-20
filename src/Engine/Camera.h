#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

enum class CameraPreset {
    CINEMATIC_EDGE_ON,
    POLAR_OVERHEAD,
    PERSPECTIVE_45,
    PHOTON_RING_CLOSEUP
};

enum class CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    Camera(glm::vec3 target = glm::vec3(0.0f), float distance = 15.0f, float yaw = 0.0f, float pitch = 15.0f);
    ~Camera() = default;

    // Updates camera based on mouse and keyboard inputs
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMousePan(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);
    void ProcessKeyboard(CameraDirection direction, float deltaTime);
    void ProcessKeyboard(int direction, float deltaTime); // Backwards compatibility

    // Set preset view
    void SetPreset(CameraPreset preset);

    // Matrices and vectors
    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetPosition() const;
    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;
    glm::vec3 GetTarget() const { return m_target; }

    float GetYaw() const { return m_yaw; }
    void SetYaw(float yaw) { m_yaw = yaw; UpdateCameraVectors(); }

    float GetPitch() const { return m_pitch; }
    void SetPitch(float pitch) { m_pitch = pitch; UpdateCameraVectors(); }

    float GetDistance() const { return m_distance; }
    void SetDistance(float dist) { m_distance = glm::clamp(dist, 3.5f, 100.0f); UpdateCameraVectors(); }

    float GetFOV() const { return m_fov; }
    void SetFOV(float fov) { m_fov = glm::clamp(fov, 15.0f, 120.0f); }

    bool IsFreeFly() const { return m_freeFly; }
    void SetFreeFly(bool free) { m_freeFly = free; }

private:
    void UpdateCameraVectors();

    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_forward;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // Spherical coordinates
    float m_yaw;      // degrees
    float m_pitch;    // degrees
    float m_distance; // distance from target
    float m_fov;      // vertical FOV in degrees

    // Settings
    float m_orbitSpeed = 0.25f;
    float m_panSpeed = 0.02f;
    float m_zoomSpeed = 1.0f;
    float m_moveSpeed = 10.0f;

    bool m_freeFly = false;
};
