#include "BlackHole.h"
#include <algorithm>

BlackHole::BlackHole(float mass, float spin, glm::vec3 origin)
    : m_mass(std::max(0.001f, mass)),
      m_spin(std::clamp(spin, -0.998f, 0.998f)),
      m_origin(origin),
      m_spinAxis(glm::vec3(0.0f, 1.0f, 0.0f)),
      m_metric(std::abs(spin) < 0.0001f ? BlackHoleMetric::SCHWARZSCHILD : BlackHoleMetric::KERR),
      m_savedKerrSpin(std::abs(spin) < 0.0001f ? 0.75f : std::clamp(spin, -0.998f, 0.998f))
{
    UpdateParameters();
}

BlackHole::~BlackHole() {
}

void BlackHole::SetMass(float mass) {
    m_mass = std::max(0.001f, mass);
    UpdateParameters();
}

void BlackHole::SetSpin(float spin) {
    m_spin = std::clamp(spin, -0.998f, 0.998f);
    if (std::abs(m_spin) > 0.0001f) {
        m_savedKerrSpin = m_spin;
        m_metric = BlackHoleMetric::KERR;
    } else {
        m_metric = BlackHoleMetric::SCHWARZSCHILD;
    }
    UpdateParameters();
}

void BlackHole::SetSchwarzschild(bool isSchwarzschild) {
    SetMetric(isSchwarzschild ? BlackHoleMetric::SCHWARZSCHILD : BlackHoleMetric::KERR);
}

void BlackHole::SetMetric(BlackHoleMetric metric) {
    m_metric = metric;
    if (m_metric == BlackHoleMetric::SCHWARZSCHILD) {
        if (std::abs(m_spin) > 0.0001f) {
            m_savedKerrSpin = m_spin;
        }
        m_spin = 0.0f;
    } else {
        if (std::abs(m_savedKerrSpin) < 0.0001f) {
            m_savedKerrSpin = 0.75f;
        }
        m_spin = m_savedKerrSpin;
    }
    UpdateParameters();
}

float BlackHole::GetEventHorizonRadius() const {
    return m_eventHorizonRadius;
}

float BlackHole::GetErgosphereRadius(float theta) const {
    float cosTheta = std::cos(theta);
    float a = m_spin * m_mass;
    float discriminant = std::max(0.0f, m_mass * m_mass - a * a * cosTheta * cosTheta);
    return m_mass + std::sqrt(discriminant);
}

float BlackHole::GetPhotonSphereRadius() const {
    return m_photonSphereRadius;
}

float BlackHole::GetISCORadius() const {
    return m_iscoRadius;
}

float BlackHole::GetCriticalImpactParameter() const {
    // For Schwarzschild (spin = 0): b_c = 3 * sqrt(3) * M
    // For Kerr equatorial prograde: decreases with spin
    float a = std::abs(m_spin);
    return m_mass * (3.0f * std::sqrt(3.0f) - a * 1.5f);
}

void BlackHole::UpdateParameters() {
    float a = m_spin; // dimensionless spin parameter
    float a2 = a * a;

    // Kerr horizon: r_H = M * (1 + sqrt(1 - a^2))
    m_eventHorizonRadius = m_mass * (1.0f + std::sqrt(std::max(0.0f, 1.0f - a2)));

    // Kerr equatorial photon sphere: r_ph = 2M * (1 + cos(2/3 * acos(-a)))
    // Prograde orbit
    float psi = std::acos(std::clamp(-a, -1.0f, 1.0f));
    m_photonSphereRadius = 2.0f * m_mass * (1.0f + std::cos(2.0f / 3.0f * psi));

    // Bardeen, Press, Teukolsky (1972) ISCO formula:
    float sign = (a >= 0.0f) ? 1.0f : -1.0f;
    float absA = std::abs(a);
    float z1 = 1.0f + std::cbrt(1.0f - absA * absA) * (std::cbrt(1.0f + absA) + std::cbrt(1.0f - absA));
    float z2 = std::sqrt(3.0f * absA * absA + z1 * z1);
    float iscoTerm = std::sqrt(std::max(0.0f, (3.0f - z1) * (3.0f + z1 + 2.0f * z2)));
    m_iscoRadius = m_mass * (3.0f + z2 - sign * iscoTerm);
}