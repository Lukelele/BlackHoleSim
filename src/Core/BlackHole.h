#pragma once

#include <glm/glm.hpp>
#include <cmath>

enum class BlackHoleMetric {
    SCHWARZSCHILD,
    KERR
};

class BlackHole {
public:
    // mass M, spin a in [-0.998, 0.998] (in geometric units G=c=1)
    BlackHole(float mass = 1.0f, float spin = 0.0f, glm::vec3 origin = glm::vec3(0.0f));
    ~BlackHole();

    // Getters and setters
    float GetMass() const { return m_mass; }
    void SetMass(float mass);

    float GetSpin() const { return m_spin; }
    void SetSpin(float spin);

    // Metric type toggle
    bool IsSchwarzschild() const { return m_metric == BlackHoleMetric::SCHWARZSCHILD || std::abs(m_spin) < 0.0001f; }
    void SetSchwarzschild(bool isSchwarzschild);

    BlackHoleMetric GetMetric() const { return m_metric; }
    void SetMetric(BlackHoleMetric metric);

    float GetSavedKerrSpin() const { return m_savedKerrSpin; }

    glm::vec3 GetOrigin() const { return m_origin; }
    void SetOrigin(const glm::vec3& origin) { m_origin = origin; }

    glm::vec3 GetSpinAxis() const { return m_spinAxis; }
    void SetSpinAxis(const glm::vec3& axis) { m_spinAxis = glm::normalize(axis); }

    // Relativistic Horizons and Radii (Geometric units G = c = 1)
    // Schwarzschild radius: r_s = 2M
    float GetSchwarzschildRadius() const { return 2.0f * m_mass; }

    // Kerr Event Horizon: r_H = M + sqrt(M^2 - a^2)
    float GetEventHorizonRadius() const;

    // Ergosphere boundary at polar angle theta: r_E(theta) = M + sqrt(M^2 - a^2 * cos^2(theta))
    float GetErgosphereRadius(float theta) const;

    // Photon sphere radius: 3M for Schwarzschild, splits into prograde/retrograde for Kerr
    float GetPhotonSphereRadius() const;

    // Innermost Stable Circular Orbit (ISCO): 6M for Schwarzschild
    float GetISCORadius() const;

    // Critical impact parameter for capture: b_c = 3*sqrt(3)*M for Schwarzschild
    float GetCriticalImpactParameter() const;

    // Accretion disk defaults
    float GetDiskInnerRadius() const { return GetISCORadius(); }
    float GetDiskOuterRadius() const { return 18.0f * m_mass; }

private:
    void UpdateParameters();

    float m_mass;
    float m_spin; // dimensionless spin parameter a/M (-1 < a < 1)
    glm::vec3 m_origin;
    glm::vec3 m_spinAxis;

    BlackHoleMetric m_metric;
    float m_savedKerrSpin;

    float m_eventHorizonRadius;
    float m_photonSphereRadius;
    float m_iscoRadius;
};