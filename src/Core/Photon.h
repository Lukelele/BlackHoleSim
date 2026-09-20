#pragma once

#include <glm/glm.hpp>
#include <vector>

enum class PhotonStatus {
    IN_FLIGHT,
    CAPTURED_HORIZON,
    ESCAPED_INFINITY
};

class Photon {
public:
    Photon(glm::vec3 position = glm::vec3(0.0f), glm::vec3 velocity = glm::vec3(0.0f, 0.0f, -1.0f), bool recordTrajectory = true);
    ~Photon() = default;

    // State getters and setters
    glm::vec3 GetPosition() const { return m_position; }
    void SetPosition(const glm::vec3& pos) { m_position = pos; }

    glm::vec3 GetVelocity() const { return m_velocity; }
    void SetVelocity(const glm::vec3& vel) { m_velocity = vel; }

    float GetAffineParam() const { return m_lambda; }
    void SetAffineParam(float lambda) { m_lambda = lambda; }

    PhotonStatus GetStatus() const { return m_status; }
    void SetStatus(PhotonStatus status) { m_status = status; }

    glm::vec3 GetColor() const { return m_color; }
    void SetColor(const glm::vec3& color) { m_color = color; }

    float GetOpticalDepth() const { return m_opticalDepth; }
    void AddOpticalDepth(float dtau) { m_opticalDepth += dtau; }

    // Constants of motion
    // Angular momentum vector L = r x v
    glm::vec3 GetAngularMomentum() const { return glm::cross(m_position, m_velocity); }
    float GetAngularMomentumMagnitude() const { return glm::length(GetAngularMomentum()); }

    // Distance from coordinate origin
    float GetRadius() const { return glm::length(m_position); }

    // Trajectory recording for debugging / CPU rendering
    const std::vector<glm::vec3>& GetTrajectory() const { return m_trajectory; }
    bool GetRecordTrajectory() const { return m_recordTrajectory; }
    void SetRecordTrajectory(bool record) { m_recordTrajectory = record; }
    void RecordPosition() { m_trajectory.push_back(m_position); }
    void ClearTrajectory() { m_trajectory.clear(); }

    void Step(const glm::vec3& newPos, const glm::vec3& newVel, float dlambda);

private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_lambda = 0.0f;
    float m_opticalDepth = 0.0f;
    glm::vec3 m_color = glm::vec3(0.0f);
    PhotonStatus m_status = PhotonStatus::IN_FLIGHT;

    std::vector<glm::vec3> m_trajectory;
    bool m_recordTrajectory = true;
};
