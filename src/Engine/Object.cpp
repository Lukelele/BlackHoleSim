#include "Object.h"
#include <utility>


Object::Object(Mesh* mesh)
    : m_mesh(mesh), 
      m_VBO(m_mesh ? m_mesh->Vertices.data() : nullptr, m_mesh ? m_mesh->Vertices.size() * sizeof(glm::vec3) : 0),
      m_VAO(),
      m_position(0.0f, 0.0f, 0.0f)
{
    if (m_mesh)
    {
        m_VAO.AddBuffer(m_VBO, VertexAttribute(0, 3, GL_FLOAT));
    }
}

Object::~Object()
{
    if (m_mesh)
    {
        delete m_mesh;
        m_mesh = nullptr;
    }
}

Object::Object(Object&& other) noexcept
    : m_mesh(other.m_mesh),
      m_VBO(std::move(other.m_VBO)),
      m_VAO(std::move(other.m_VAO)),
      m_position(other.m_position)
{
    other.m_mesh = nullptr;
}

Object& Object::operator=(Object&& other) noexcept
{
    if (this != &other)
    {
        if (m_mesh)
            delete m_mesh;

        m_mesh = other.m_mesh;
        m_VBO = std::move(other.m_VBO);
        m_VAO = std::move(other.m_VAO);
        m_position = other.m_position;

        other.m_mesh = nullptr;
    }
    return *this;
}
