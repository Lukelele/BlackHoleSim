#pragma once

#include <vector>
#include "VertexArray.h"
#include "Mesh.h"

#include <glm/glm.hpp>

class Object {
public:
    Object(Mesh* mesh);
    ~Object();

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&& other) noexcept;
    Object& operator=(Object&& other) noexcept;

    Mesh* GetMesh() { return m_mesh; }
    VertexArray &GetVertexArray() { return m_VAO; }
    VertexBuffer &GetVertexBuffer() { return m_VBO; }

private:
    Mesh* m_mesh;
    VertexBuffer m_VBO;
    VertexArray m_VAO;

    glm::vec3 m_position;
};