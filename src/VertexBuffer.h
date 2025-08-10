#pragma once

#include <glad/glad.h>


class VertexBuffer
{
public:
    VertexBuffer(const void* data, unsigned int size, GLenum bufferType = GL_ARRAY_BUFFER);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:
    GLuint m_bufferID;
    GLenum m_bufferType;
};
