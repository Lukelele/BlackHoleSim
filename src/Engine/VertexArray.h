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
private:
    GLuint m_RendererID;
};
