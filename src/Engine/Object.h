#pragma once

#include <vector>
#include "VertexArray.h"
#include "Mesh.h"


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
    VertexArray m_va;
    VertexBuffer m_VBO;
    VertexArray m_VAO;
};