#include "VertexArray.h"


VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
    Bind();
}

VertexArray::~VertexArray()
{
    Unbind();
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddBuffer(VertexBuffer &buffer, VertexAttribute attribute)
{
    Bind();
    buffer.Bind();
    glVertexAttribPointer(attribute.index, attribute.countsPerVertex, attribute.dataVariableType, attribute.normalised, attribute.stride, attribute.startPointer);
    glEnableVertexAttribArray(attribute.index);

    if (attribute.index == 0) {
        if (attribute.stride == 0) {
            m_vertexCount = buffer.GetSize() / (attribute.countsPerVertex * sizeof(float));
        } else {
            m_vertexCount = buffer.GetSize() / attribute.stride;
        }
    }
}
