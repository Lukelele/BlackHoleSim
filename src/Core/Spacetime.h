#pragma once

#include "BlackHole.h"
#include "Photon.h"
#include <glm/glm.hpp>
#include <vector>

class Spacetime {
public:
    Spacetime(BlackHole blackHole = BlackHole(1.0f, 0.0f));
    ~Spacetime();

    const BlackHole& GetBlackHole() const { return m_blackHole; }
    BlackHole& GetBlackHole() { return m_blackHole; }
    void SetBlackHole(const BlackHole& bh) { m_blackHole = bh; }

    // Geodesic acceleration vector in 3D: a = d^2r / dlambda^2
    glm::vec3 CalculateGeodesicAcceleration(const glm::vec3& r, const glm::vec3& v) const;

    // Numerical Runge-Kutta 4th Order step
    void StepRK4(glm::vec3& pos, glm::vec3& vel, float dlambda) const;

    // Full CPU photon trace (for analytical checking and debugging)
    PhotonStatus TracePhoton(Photon& photon, float maxDistance = 60.0f, int maxSteps = 1500) const;

    // Relativistic checks and analytical benchmarks
    float AnalyticalWeakFieldDeflection(float impactParameter) const;
    bool IsImpactParameterCaptured(float impactParameter) const;

    // Accretion disk physics helpers
    float GetKeplerianOrbitalVelocity(float radius) const;
    float GetDopplerFactor(const glm::vec3& pos, const glm::vec3& rayDirection) const;
    float GetGravitationalRedshift(float radius) const;
    glm::vec3 GetBlackbodyColor(float temperature) const;

private:
    BlackHole m_blackHole;
};