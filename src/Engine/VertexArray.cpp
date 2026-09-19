#include "VertexArray.h"


VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
    Bind();
}

VertexArray::~VertexArray()
{
    if (m_RendererID)
    {
        Unbind();
        glDeleteVertexArrays(1, &m_RendererID);
    }
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_RendererID(other.m_RendererID)
{
    other.m_RendererID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other)
    {
        if (m_RendererID)
        {
            Unbind();
            glDeleteVertexArrays(1, &m_RendererID);
        }
        m_RendererID = other.m_RendererID;
        other.m_RendererID = 0;
    }
    return *this;
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
