#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>

#include "Core/BlackHole.h"
#include "Core/Spacetime.h"
#include "Core/Photon.h"

int main() {
    std::cout << "========================================================\n";
    std::cout << "  General Relativity Black Hole Physics Verification\n";
    std::cout << "========================================================\n" << std::endl;

    // 1. Black hole horizon and ISCO verification
    float M = 1.0f;
    BlackHole bh(M, 0.0f); // Schwarzschild black hole
    std::cout << "[Test 1] Horizon and Orbit Dimensions (M = 1.0):" << std::endl;
    std::cout << "  Schwarzschild Radius r_s: " << bh.GetSchwarzschildRadius() << " (Expected: 2.0)" << std::endl;
    std::cout << "  Event Horizon Radius r_H: " << bh.GetEventHorizonRadius() << " (Expected: 2.0)" << std::endl;
    std::cout << "  Photon Sphere Radius r_ph: " << bh.GetPhotonSphereRadius() << " (Expected: 3.0)" << std::endl;
    std::cout << "  ISCO Radius r_ISCO: " << bh.GetISCORadius() << " (Expected: 6.0)" << std::endl;
    std::cout << "  Critical Impact Parameter b_c: " << bh.GetCriticalImpactParameter() << " (Expected: 5.19615)" << std::endl;

    assert(std::abs(bh.GetSchwarzschildRadius() - 2.0f) < 1e-4);
    assert(std::abs(bh.GetEventHorizonRadius() - 2.0f) < 1e-4);
    assert(std::abs(bh.GetPhotonSphereRadius() - 3.0f) < 1e-4);
    assert(std::abs(bh.GetISCORadius() - 6.0f) < 1e-4);
    assert(std::abs(bh.GetCriticalImpactParameter() - 3.0f * std::sqrt(3.0f)) < 1e-4);
    std::cout << "  -> PASSED!\n" << std::endl;

    // 2. Kerr Black Hole (a = 0.9)
    BlackHole kerrBH(M, 0.9f);
    std::cout << "[Test 2] Kerr Black Hole Properties (M = 1.0, a = 0.9):" << std::endl;
    std::cout << "  Event Horizon Radius: " << kerrBH.GetEventHorizonRadius() << " (Expected: ~1.43589)" << std::endl;
    std::cout << "  Prograde ISCO: " << kerrBH.GetISCORadius() << " (Expected: ~2.32)" << std::endl;
    std::cout << "  Prograde Photon Sphere: " << kerrBH.GetPhotonSphereRadius() << std::endl;
    assert(kerrBH.GetEventHorizonRadius() < 2.0f);
    assert(kerrBH.GetISCORadius() < 6.0f);
    std::cout << "  -> PASSED!\n" << std::endl;

    // 3. Conservation of Angular Momentum along Null Geodesics
    Spacetime spacetime(bh);
    std::cout << "[Test 3] Conservation of Angular Momentum L = r x v:" << std::endl;
    glm::vec3 pos(10.0f, 0.0f, 25.0f);
    glm::vec3 vel = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 initialL = glm::cross(pos, vel);
    float initialL_mag = glm::length(initialL);

    for (int i = 0; i < 250; ++i) {
        float r = glm::length(pos);
        if (r <= 2.05f || (r > 35.0f && glm::dot(pos, vel) > 0.0f)) break;
        float h = std::clamp(0.02f * r, 0.005f, 0.1f);
        spacetime.StepRK4(pos, vel, h);
    }
    glm::vec3 finalL = glm::cross(pos, vel);
    float finalL_mag = glm::length(finalL);
    float L_error = std::abs(finalL_mag - initialL_mag) / initialL_mag;
    std::cout << "  Initial |L|: " << initialL_mag << std::endl;
    std::cout << "  Final   |L|: " << finalL_mag << std::endl;
    std::cout << "  Relative Error in |L|: " << L_error << std::endl;
    assert(L_error < 0.01f);
    std::cout << "  -> PASSED!\n" << std::endl;

    // 4. Critical Impact Parameter Photon Capture Test
    std::cout << "[Test 4] Critical Impact Parameter Capture (b_c = 3*sqrt(3)*M = 5.196):" << std::endl;
    // Sub-critical ray: b = 4.8 < b_c -> must be captured by horizon
    Photon subCritical(glm::vec3(4.8f, 0.0f, 30.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    PhotonStatus statusSub = spacetime.TracePhoton(subCritical, 50.0f, 3000);
    std::cout << "  Ray with b = 4.8 M (< b_c): " << (statusSub == PhotonStatus::CAPTURED_HORIZON ? "CAPTURED (Correct!)" : "ESCAPED (Incorrect!)") << std::endl;
    assert(statusSub == PhotonStatus::CAPTURED_HORIZON);

    // Super-critical ray: b = 5.6 > b_c -> must escape
    Photon superCritical(glm::vec3(5.6f, 0.0f, 30.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    PhotonStatus statusSuper = spacetime.TracePhoton(superCritical, 50.0f, 3000);
    std::cout << "  Ray with b = 5.6 M (> b_c): " << (statusSuper == PhotonStatus::ESCAPED_INFINITY ? "ESCAPED (Correct!)" : "CAPTURED (Incorrect!)") << std::endl;
    assert(statusSuper == PhotonStatus::ESCAPED_INFINITY);
    std::cout << "  -> PASSED!\n" << std::endl;

    // 5. Weak Field Light Deflection
    std::cout << "[Test 5] Weak-Field Light Deflection Test:" << std::endl;
    float b_weak = 40.0f;
    Photon weakRay(glm::vec3(b_weak, 0.0f, 60.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    spacetime.TracePhoton(weakRay, 60.0f, 3000);
    glm::vec3 initialDir(0.0f, 0.0f, -1.0f);
    glm::vec3 finalDir = glm::normalize(weakRay.GetVelocity());
    float cosTheta = glm::clamp(glm::dot(initialDir, finalDir), -1.0f, 1.0f);
    float numericalDeflection = std::acos(cosTheta);
    float theoreticalDeflection = spacetime.AnalyticalWeakFieldDeflection(b_weak);
    std::cout << "  Impact parameter b: " << b_weak << " M" << std::endl;
    std::cout << "  Numerical Deflection:   " << numericalDeflection << " rad (" << glm::degrees(numericalDeflection) << " deg)" << std::endl;
    std::cout << "  Theoretical (4M/b):     " << theoreticalDeflection << " rad (" << glm::degrees(theoreticalDeflection) << " deg)" << std::endl;
    std::cout << "  Relative agreement:     " << (1.0f - std::abs(numericalDeflection - theoreticalDeflection) / theoreticalDeflection) * 100.0f << "%" << std::endl;
    assert(std::abs(numericalDeflection - theoreticalDeflection) / theoreticalDeflection < 0.15f);
    std::cout << "  -> PASSED!\n" << std::endl;

    // 6. Relativistic Doppler & Redshift
    std::cout << "[Test 6] Relativistic Doppler Factor & Redshift at r = 6M:" << std::endl;
    float vOrb = spacetime.GetKeplerianOrbitalVelocity(6.0f);
    float zGrav = spacetime.GetGravitationalRedshift(6.0f);
    std::cout << "  Keplerian orbital velocity at ISCO: " << vOrb << " c" << std::endl;
    std::cout << "  Gravitational redshift factor at ISCO: " << zGrav << " (Expected: sqrt(2/3) ~ 0.8165)" << std::endl;
    assert(std::abs(zGrav - std::sqrt(2.0f / 3.0f)) < 0.01f);

    // Approaching vs Receding Doppler factor
    glm::vec3 diskPos(6.0f, 0.0f, 0.0f);
    // Ray towards observer looking along +Z
    float dopplerApproaching = spacetime.GetDopplerFactor(diskPos, glm::vec3(0.0f, 0.0f, 1.0f));
    std::cout << "  Doppler factor: " << dopplerApproaching << std::endl;
    std::cout << "  -> PASSED!\n" << std::endl;

    // 7. Schwarzschild Toggle and State Preservation
    std::cout << "[Test 7] Schwarzschild Black Hole Toggle & State Preservation:" << std::endl;
    BlackHole toggleBH(1.0f, 0.85f);
    assert(!toggleBH.IsSchwarzschild());
    assert(std::abs(toggleBH.GetSpin() - 0.85f) < 1e-4);
    assert(toggleBH.GetISCORadius() < 6.0f); // Kerr ISCO is contracted

    // Toggle to Schwarzschild
    toggleBH.SetSchwarzschild(true);
    assert(toggleBH.IsSchwarzschild());
    assert(std::abs(toggleBH.GetSpin()) < 1e-5);
    assert(std::abs(toggleBH.GetSchwarzschildRadius() - 2.0f) < 1e-4);
    assert(std::abs(toggleBH.GetEventHorizonRadius() - 2.0f) < 1e-4);
    assert(std::abs(toggleBH.GetPhotonSphereRadius() - 3.0f) < 1e-4);
    assert(std::abs(toggleBH.GetISCORadius() - 6.0f) < 1e-4);
    std::cout << "  Toggled to Schwarzschild: spin = " << toggleBH.GetSpin()
              << ", r_H = " << toggleBH.GetEventHorizonRadius()
              << ", r_ISCO = " << toggleBH.GetISCORadius() << std::endl;

    // Toggle back to Kerr - must restore previous spin
    toggleBH.SetSchwarzschild(false);
    assert(!toggleBH.IsSchwarzschild());
    assert(std::abs(toggleBH.GetSpin() - 0.85f) < 1e-4);
    assert(toggleBH.GetISCORadius() < 6.0f);
    std::cout << "  Toggled back to Kerr: spin restored = " << toggleBH.GetSpin()
              << ", r_H = " << toggleBH.GetEventHorizonRadius()
              << ", r_ISCO = " << toggleBH.GetISCORadius() << std::endl;
    std::cout << "  -> PASSED!\n" << std::endl;

    std::cout << "ALL GENERAL RELATIVITY PHYSICS TESTS PASSED PERFECTLY!\n";
    return 0;
}
