#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum bufferType, GLenum drawUsage)
    : m_bufferType(bufferType), m_size(size)
{
    glGenBuffers(1, &m_bufferID);
    glBindBuffer(bufferType, m_bufferID);
    glBufferData(bufferType, size, data, drawUsage);
}

VertexBuffer::~VertexBuffer()
{
    if (m_bufferID)
        glDeleteBuffers(1, &m_bufferID);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_bufferID(other.m_bufferID), m_bufferType(other.m_bufferType), m_size(other.m_size)
{
    other.m_bufferID = 0;
    other.m_size = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
    if (this != &other)
    {
        if (m_bufferID)
            glDeleteBuffers(1, &m_bufferID);

        m_bufferID = other.m_bufferID;
        m_bufferType = other.m_bufferType;
        m_size = other.m_size;

        other.m_bufferID = 0;
        other.m_size = 0;
    }
    return *this;
}

void VertexBuffer::Bind() const
{
    glBindBuffer(m_bufferType, m_bufferID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(m_bufferType, 0);
}
