#pragma once

#include <glad/glad.h>
#include "VertexBuffer.h"
#include "VertexAttribute.h"


class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void Bind() const;
    void Unbind() const;
    void AddBuffer(VertexBuffer &buffer, VertexAttribute attribute);
private:
    GLuint m_RendererID;
};
