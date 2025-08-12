#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>


class Mesh {
public:
    Mesh();
    Mesh(glm::vec3 vertices[], unsigned int vertexCount);
    Mesh(std::vector<glm::vec3> vertices);
    ~Mesh();

    std::vector<glm::vec3> Vertices;
    unsigned int VertexCount;
};
