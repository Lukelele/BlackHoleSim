#include "Camera.h"
#include <cmath>
#include <algorithm>

Camera::Camera(glm::vec3 target, float distance, float yaw, float pitch)
    : m_target(target),
      m_worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
      m_yaw(yaw),
      m_pitch(pitch),
      m_distance(distance),
      m_fov(55.0f),
      m_freeFly(false)
{
    UpdateCameraVectors();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_orbitSpeed;
    yoffset *= m_orbitSpeed;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch) {
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }

    UpdateCameraVectors();
}

void Camera::ProcessMousePan(float xoffset, float yoffset) {
    float dx = -xoffset * m_panSpeed * (m_distance * 0.05f);
    float dy = yoffset * m_panSpeed * (m_distance * 0.05f);

    m_target += m_right * dx + m_up * dy;
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    m_distance -= yoffset * m_zoomSpeed;
    m_distance = std::clamp(m_distance, 3.5f, 100.0f);
    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(CameraDirection direction, float deltaTime) {
    float velocity = m_moveSpeed * deltaTime;
    switch (direction) {
        case CameraDirection::FORWARD:
            m_distance = std::clamp(m_distance - velocity, 3.5f, 100.0f);
            break;
        case CameraDirection::BACKWARD:
            m_distance = std::clamp(m_distance + velocity, 3.5f, 100.0f);
            break;
        case CameraDirection::LEFT:
            m_yaw -= velocity * 5.0f;
            break;
        case CameraDirection::RIGHT:
            m_yaw += velocity * 5.0f;
            break;
        case CameraDirection::UP:
            m_pitch = std::clamp(m_pitch + velocity * 5.0f, -89.0f, 89.0f);
            break;
        case CameraDirection::DOWN:
            m_pitch = std::clamp(m_pitch - velocity * 5.0f, -89.0f, 89.0f);
            break;
    }
    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    if (direction >= 0 && direction <= 5) {
        ProcessKeyboard(static_cast<CameraDirection>(direction), deltaTime);
    }
}

void Camera::SetPreset(CameraPreset preset) {
    m_target = glm::vec3(0.0f);
    switch (preset) {
        case CameraPreset::CINEMATIC_EDGE_ON:
            m_distance = 18.0f;
            m_yaw = 0.0f;
            m_pitch = 6.0f;
            m_fov = 50.0f;
            break;
        case CameraPreset::POLAR_OVERHEAD:
            m_distance = 22.0f;
            m_yaw = 0.0f;
            m_pitch = 85.0f;
            m_fov = 55.0f;
            break;
        case CameraPreset::PERSPECTIVE_45:
            m_distance = 16.0f;
            m_yaw = -30.0f;
            m_pitch = 25.0f;
            m_fov = 55.0f;
            break;
        case CameraPreset::PHOTON_RING_CLOSEUP:
            m_distance = 7.0f;
            m_yaw = 15.0f;
            m_pitch = 10.0f;
            m_fov = 40.0f;
            break;
    }
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::vec3 Camera::GetPosition() const {
    return m_position;
}

glm::vec3 Camera::GetForward() const {
    return m_forward;
}

glm::vec3 Camera::GetRight() const {
    return m_right;
}

glm::vec3 Camera::GetUp() const {
    return m_up;
}

void Camera::UpdateCameraVectors() {
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    // Compute camera position on sphere centered at m_target
    m_position.x = m_target.x + m_distance * std::cos(pitchRad) * std::sin(yawRad);
    m_position.y = m_target.y + m_distance * std::sin(pitchRad);
    m_position.z = m_target.z + m_distance * std::cos(pitchRad) * std::cos(yawRad);

    m_forward = glm::normalize(m_target - m_position);
    m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}
