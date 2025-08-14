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
    glEnableVertexAttribArray(attribute.Index);
    glVertexAttribPointer(attribute.Index, attribute.CountPerVertex, attribute.DataType, attribute.Normalised, attribute.Stride, attribute.StartPointer);
}
