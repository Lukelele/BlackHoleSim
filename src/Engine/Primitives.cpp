#include "Primitives.h"

Circle::Circle(float radius, glm::vec3 origin, unsigned int segments)
    : Radius(radius), Origin(origin), Segments(segments)
{
    Vertices.resize(segments * 3);

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
