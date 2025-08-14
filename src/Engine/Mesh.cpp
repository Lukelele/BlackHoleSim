#include "Mesh.h"

Mesh::Mesh(GLenum drawMode)
    : VertexCount(0), Vertices(), DrawMode(drawMode)
{
}

Mesh::Mesh(glm::vec3 vertices[], unsigned int vertexCount, GLenum drawMode)
    : Vertices(vertices, vertices + vertexCount), VertexCount(vertexCount), DrawMode(drawMode)
{
}

Mesh::Mesh(std::vector<glm::vec3> vertices, GLenum drawMode)
    : Vertices(std::move(vertices)), VertexCount(static_cast<unsigned int>(Vertices.size())), DrawMode(drawMode)
{
}

Mesh::~Mesh()
{
    Vertices.clear();
}
