#pragma once

#include <vector>
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>


class Mesh {
public:
    Mesh(GLenum drawMode = GL_TRIANGLES);
    Mesh(glm::vec3 vertices[], unsigned int vertexCount, GLenum drawMode = GL_TRIANGLES);
    Mesh(std::vector<glm::vec3> vertices, GLenum drawMode = GL_TRIANGLES);
    virtual ~Mesh();

    std::vector<glm::vec3> Vertices;
    unsigned int VertexCount;
    GLenum DrawMode;
};
