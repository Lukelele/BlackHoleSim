#include "Spacetime.h"
#include <cmath>
#include <algorithm>

Spacetime::Spacetime(BlackHole blackHole)
    : m_blackHole(blackHole)
{
}

Spacetime::~Spacetime() {
}

glm::vec3 Spacetime::CalculateGeodesicAcceleration(const glm::vec3& r, const glm::vec3& v) const {
    glm::vec3 relPos = r - m_blackHole.GetOrigin();
    float rLen = glm::length(relPos);
    if (rLen < 0.0001f) {
        return glm::vec3(0.0f);
    }

    float M = m_blackHole.GetMass();
    glm::vec3 L = glm::cross(relPos, v);
    float L2 = glm::dot(L, L);

    // Exact spatial geodesic equation in affine coordinates for Schwarzschild:
    // d^2 r / dlambda^2 = - (3 * M * L^2 / r^5) * r
    float r5 = rLen * rLen * rLen * rLen * rLen;
    glm::vec3 a_schwarzschild = -((3.0f * M * L2) / r5) * relPos;

    // Kerr frame-dragging correction (Lense-Thirring effect)
    float a = m_blackHole.GetSpin();
    if (std::abs(a) > 0.001f) {
        glm::vec3 S = (a * M) * m_blackHole.GetSpinAxis();
        float r3 = rLen * rLen * rLen;
        glm::vec3 omega_FD = (2.0f / r3) * S - (3.0f * glm::dot(S, relPos) / r5) * relPos;
        glm::vec3 a_kerr = 2.0f * glm::cross(v, omega_FD);
        return a_schwarzschild + a_kerr;
    }

    return a_schwarzschild;
}

void Spacetime::StepRK4(glm::vec3& pos, glm::vec3& vel, float dlambda) const {
    // k1
    glm::vec3 k1_v = CalculateGeodesicAcceleration(pos, vel);
    glm::vec3 k1_r = vel;

    // k2
    glm::vec3 r2 = pos + 0.5f * dlambda * k1_r;
    glm::vec3 v2 = vel + 0.5f * dlambda * k1_v;
    glm::vec3 k2_v = CalculateGeodesicAcceleration(r2, v2);
    glm::vec3 k2_r = v2;

    // k3
    glm::vec3 r3 = pos + 0.5f * dlambda * k2_r;
    glm::vec3 v3 = vel + 0.5f * dlambda * k2_v;
    glm::vec3 k3_v = CalculateGeodesicAcceleration(r3, v3);
    glm::vec3 k3_r = v3;

    // k4
    glm::vec3 r4 = pos + dlambda * k3_r;
    glm::vec3 v4 = vel + dlambda * k3_v;
    glm::vec3 k4_v = CalculateGeodesicAcceleration(r4, v4);
    glm::vec3 k4_r = v4;

    pos += (dlambda / 6.0f) * (k1_r + 2.0f * k2_r + 2.0f * k3_r + k4_r);
    vel += (dlambda / 6.0f) * (k1_v + 2.0f * k2_v + 2.0f * k3_v + k4_v);
}

PhotonStatus Spacetime::TracePhoton(Photon& photon, float maxDistance, int maxSteps) const {
    glm::vec3 pos = photon.GetPosition();
    glm::vec3 vel = photon.GetVelocity();
    float rH = m_blackHole.GetEventHorizonRadius();

    for (int step = 0; step < maxSteps; ++step) {
        float r = glm::length(pos - m_blackHole.GetOrigin());

        // Check if swallowed by event horizon
        if (r <= rH * 1.01f) {
            photon.SetPosition(pos);
            photon.SetVelocity(vel);
            photon.SetStatus(PhotonStatus::CAPTURED_HORIZON);
            return PhotonStatus::CAPTURED_HORIZON;
        }

        // Check if escaped to celestial sphere
        if (r > maxDistance && glm::dot(pos - m_blackHole.GetOrigin(), vel) > 0.0f) {
            photon.SetPosition(pos);
            photon.SetVelocity(vel);
            photon.SetStatus(PhotonStatus::ESCAPED_INFINITY);
            return PhotonStatus::ESCAPED_INFINITY;
        }

        // Adaptive step size: finer near the horizon/photon sphere, coarser far away
        float dlambda = std::clamp(0.04f * r, 0.01f, 0.25f);
        StepRK4(pos, vel, dlambda);
        photon.Step(pos, vel, dlambda);
    }

    photon.SetPosition(pos);
    photon.SetVelocity(vel);
    photon.SetStatus(PhotonStatus::IN_FLIGHT);
    return PhotonStatus::IN_FLIGHT;
}

float Spacetime::AnalyticalWeakFieldDeflection(float impactParameter) const {
    if (impactParameter <= 0.0001f) return 0.0f;
    return (4.0f * m_blackHole.GetMass()) / impactParameter;
}

bool Spacetime::IsImpactParameterCaptured(float impactParameter) const {
    return impactParameter <= m_blackHole.GetCriticalImpactParameter();
}

float Spacetime::GetKeplerianOrbitalVelocity(float radius) const {
    float M = m_blackHole.GetMass();
    if (radius <= 2.0f * M) return 0.99f;
    // v = sqrt(M / r)
    return std::min(0.99f, std::sqrt(M / radius));
}

float Spacetime::GetDopplerFactor(const glm::vec3& pos, const glm::vec3& rayDirection) const {
    glm::vec3 relPos = pos - m_blackHole.GetOrigin();
    float r = glm::length(relPos);
    if (r < 0.001f) return 1.0f;

    // Keplerian disk velocity vector in equatorial plane (cross spin axis and radial vector)
    glm::vec3 up = m_blackHole.GetSpinAxis();
    glm::vec3 tangent = glm::normalize(glm::cross(up, relPos));
    float vOrb = GetKeplerianOrbitalVelocity(r);
    glm::vec3 beta = tangent * vOrb;

    // Photon direction towards observer is -rayDirection
    glm::vec3 k = glm::normalize(-rayDirection);
    float betaDotK = glm::dot(beta, k);
    float beta2 = glm::dot(beta, beta);
    float gamma = 1.0f / std::sqrt(std::max(0.001f, 1.0f - beta2));

    // Doppler factor delta = 1 / (gamma * (1 - beta . k))
    float denom = gamma * (1.0f - betaDotK);
    if (std::abs(denom) < 0.001f) denom = 0.001f;
    return 1.0f / denom;
}

float Spacetime::GetGravitationalRedshift(float radius) const {
    float M = m_blackHole.GetMass();
    if (radius <= 2.0f * M) return 0.0f;
    return std::sqrt(1.0f - (2.0f * M) / radius);
}

glm::vec3 Spacetime::GetBlackbodyColor(float temperature) const {
    // Temperature in Kelvin, mapped to normalized RGB
    float t = temperature / 100.0f;
    float r, g, b;

    // Red
    if (t <= 66.0f) {
        r = 1.0f;
    } else {
        r = std::clamp(std::pow(t - 60.0f, -0.1332047592f) * 1.2929361860f, 0.0f, 1.0f);
    }

    // Green
    if (t <= 66.0f) {
        g = std::clamp(std::log(t) * 0.3900815788f - 0.6318414438f, 0.0f, 1.0f);
    } else {
        g = std::clamp(std::pow(t - 60.0f, -0.0755148492f) * 1.1298908609f, 0.0f, 1.0f);
    }

    // Blue
    if (t >= 66.0f) {
        b = 1.0f;
    } else if (t <= 19.0f) {
        b = 0.0f;
    } else {
        b = std::clamp(std::log(t - 10.0f) * 0.5432067891f - 1.1962540891f, 0.0f, 1.0f);
    }

    return glm::vec3(r, g, b);
}