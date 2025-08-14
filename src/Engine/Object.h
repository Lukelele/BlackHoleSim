#pragma once

#include <vector>
#include "VertexArray.h"
#include "Mesh.h"


using namespace glm;


class Object {
public:
    Object();
    Object(Mesh* mesh);
    ~Object();

    Mesh* GetMesh() { return m_mesh; }
    VertexArray &GetVertexArray() { return m_VAO; }
    VertexBuffer &GetVertexBuffer() { return m_VBO; }

private:
    Mesh* m_mesh;
    VertexArray m_VAO;
    VertexBuffer m_VBO;

    glm::vec3 m_position;
};