#pragma once

#include <vector>
#include "VertexArray.h"
#include "Mesh.h"


class Object {
public:
    Object();
    Object(glm::vec3 vertices[], unsigned int vertexCount);
    Object(std::vector<glm::vec3> vertices);
    ~Object();

    std::vector<glm::vec3> GetVertices() const { return m_mesh.Vertices; }
    unsigned int GetVertexCount() const { return m_mesh.VertexCount; }

private:
    Mesh m_mesh;
    VertexArray m_va;
    std::vector<GLuint> m_bufferIDs;
};