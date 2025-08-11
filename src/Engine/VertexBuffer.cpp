#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum bufferType)
    : m_bufferType(bufferType)
{
    glGenBuffers(1, &m_bufferID);
    glBindBuffer(bufferType, m_bufferID);
    glBufferData(bufferType, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_bufferID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(m_bufferType, m_bufferID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(m_bufferType, 0);
}
