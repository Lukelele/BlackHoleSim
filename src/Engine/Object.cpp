#include "Object.h"


Object::Object(Mesh* mesh)
    : m_mesh(mesh), 
      m_VBO(m_mesh->Vertices.data(), m_mesh->Vertices.size() * sizeof(glm::vec3)),
      m_VAO()
{
    m_VAO.AddBuffer(m_VBO, VertexAttribute(0, 3, GL_FLOAT));
}


Object::~Object()
{
    delete m_mesh;
}
