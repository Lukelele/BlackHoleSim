#pragma once

#include <vector>
#include <glm/glm.hpp>


enum class CoordinateSystem {
    CARTESIAN,
    SPHERICAL
};


class Spacetime {
public:
    Spacetime();
    ~Spacetime();

    void SchwarzschildGeodesic();

private:
    float m_lambda = 0.0f;
    float m_dlambda = 0.1f;
};