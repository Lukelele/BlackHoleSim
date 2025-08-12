#pragma once


#include <cmath>
#include "Mesh.h"
#include "glm/glm.hpp"

#define PI 3.14159265359f


class Circle: public Mesh {
public:
    Circle(float radius, glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f), unsigned int segments = 90);
    ~Circle();

    float Radius;
    glm::vec3 Origin;
    unsigned int Segments;
};