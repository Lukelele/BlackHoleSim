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

class CircleFan: public Mesh {
public:
    CircleFan(float radius, glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f), unsigned int segments = 90);
    ~CircleFan();

    float Radius;
    glm::vec3 Origin;
    unsigned int Segments;
};

class LineStrip: public Mesh {
public:
    LineStrip(glm::vec3 start, glm::vec3 end);
    ~LineStrip();

    glm::vec3 Start;
    glm::vec3 End;
};

class TriangleStrip: public Mesh {
public:
    TriangleStrip(const std::vector<glm::vec3> &vertices);
    ~TriangleStrip();
};
