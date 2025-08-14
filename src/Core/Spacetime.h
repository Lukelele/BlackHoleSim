#pragma once

#include <glm/glm.hpp>


enum class CoordinateSystem {
    CARTESIAN,
    POLAR,
    SPHERICAL
};


class Spacetime {
public:
    Spacetime();
    ~Spacetime();

private:
    CoordinateSystem coordinateType;
};