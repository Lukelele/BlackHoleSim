#include "Photon.h"

Photon::Photon(glm::vec3 position, glm::vec3 velocity, bool recordTrajectory)
    : m_position(position),
      m_velocity(velocity),
      m_lambda(0.0f),
      m_opticalDepth(0.0f),
      m_color(glm::vec3(0.0f)),
      m_status(PhotonStatus::IN_FLIGHT),
      m_recordTrajectory(recordTrajectory)
{
    if (m_recordTrajectory) {
        m_trajectory.push_back(m_position);
    }
}

void Photon::Step(const glm::vec3& newPos, const glm::vec3& newVel, float dlambda) {
    m_position = newPos;
    m_velocity = newVel;
    m_lambda += dlambda;
    if (m_recordTrajectory && m_trajectory.size() < 10000) {
        m_trajectory.push_back(m_position);
    }
}
