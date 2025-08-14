#pragma once

#include <glad/glad.h>


class VertexBuffer
{
public:
    VertexBuffer(const void* data, unsigned int size, GLenum bufferType = GL_ARRAY_BUFFER, GLenum drawUsage = GL_STATIC_DRAW);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetSize() const { return m_size; }

private:
    GLuint m_bufferID;
    GLenum m_bufferType;
    unsigned int m_size;
};
