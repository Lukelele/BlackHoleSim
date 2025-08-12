#include "Mesh.h"

Mesh::Mesh()
    : VertexCount(0), Vertices()
{
}

Mesh::Mesh(glm::vec3 vertices[], unsigned int vertexCount)
    : Vertices(vertices, vertices + vertexCount), VertexCount(vertexCount)
{
}

Mesh::Mesh(std::vector<glm::vec3> vertices)
    : Vertices(std::move(vertices)), VertexCount(static_cast<unsigned int>(Vertices.size()))
{
}

Mesh::~Mesh()
{
    Vertices.clear();
}
