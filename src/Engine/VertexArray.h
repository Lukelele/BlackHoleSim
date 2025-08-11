#pragma once

#include <glad/glad.h>
#include "VertexBuffer.h"
#include "VertexAttribute.h"


class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;
    void AddBuffer(VertexBuffer &buffer, VertexAttribute attribute);

    inline unsigned int GetVertexCount() const { return m_vertexCount; }

private:
    GLuint m_RendererID;
    int m_vertexCount;
};
