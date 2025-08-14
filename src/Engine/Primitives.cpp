#include "Primitives.h"


Circle::Circle(float radius, glm::vec3 origin, unsigned int segments)
    : Radius(radius), Origin(origin), Segments(segments)
{
    VertexCount = segments * 3; // Each segment creates a triangle with the center and two points on the circle
    Vertices.resize(VertexCount);

    for (int i = 0; i < segments; i++) {
        float angle = 2 * PI / segments * i;
        float next_angle = 2 * PI / segments * (i + 1);
        glm::vec3 p1 = origin + glm::vec3(radius * cos(angle), radius * sin(angle), 0);
        glm::vec3 p2 = origin + glm::vec3(radius * cos(next_angle), radius * sin(next_angle), 0);

        Vertices[i*3] = origin;
        Vertices[i*3+1] = p1;
        Vertices[i*3+2] = p2;
    }
}

Circle::~Circle()
{
    Vertices.clear();
    VertexCount = 0;
}

CircleFan::CircleFan(float radius, glm::vec3 origin, unsigned int segments)
    : Radius(radius), Origin(origin), Segments(segments)
{
    VertexCount = segments + 2;
    Vertices.resize(VertexCount);

    Vertices[0] = origin;

    for (int i = 0; i <= segments; i++) {
        float angle = 2 * PI / segments * i;
        glm::vec3 p = origin + glm::vec3(radius * cos(angle), radius * sin(angle), 0);
        Vertices[i + 1] = p;
    }
}

CircleFan::~CircleFan()
{
    Vertices.clear();
    VertexCount = 0;
}


LineStrip::LineStrip(glm::vec3 start, glm::vec3 end)
    : Start(start), End(end)
{
    VertexCount = 2;
    Vertices.resize(VertexCount);
    Vertices[0] = start;
    Vertices[1] = end;
}

LineStrip::~LineStrip()
{
    Vertices.clear();
    VertexCount = 0;
}
