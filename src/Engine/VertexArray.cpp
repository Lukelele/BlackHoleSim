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

    if (attribute.Index == 0) {
        if (attribute.Stride == 0) {
            unsigned int typeSize = 0;
            switch (attribute.DataType) {
                case GL_FLOAT: typeSize = sizeof(GLfloat); break;
                case GL_UNSIGNED_INT: typeSize = sizeof(GLuint); break;
                case GL_INT: typeSize = sizeof(GLint); break;
                case GL_DOUBLE: typeSize = sizeof(GLdouble); break;
                case GL_SHORT: typeSize = sizeof(GLshort); break;
                case GL_UNSIGNED_SHORT: typeSize = sizeof(GLushort); break;
                case GL_UNSIGNED_BYTE: typeSize = sizeof(GLubyte); break;
                case GL_BYTE: typeSize = sizeof(GLbyte); break;
            }
            m_vertexCount = buffer.GetSize() / (attribute.CountPerVertex * typeSize);
        } else {
            m_vertexCount = buffer.GetSize() / attribute.Stride;
        }
    }
}
