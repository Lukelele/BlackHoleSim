#pragma once

#include <glad/glad.h>
#include "VertexBuffer.h"


class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;
    void AddBuffer(const VertexBuffer& buffer, GLuint index);

private:
    GLuint m_RendererID;
};
