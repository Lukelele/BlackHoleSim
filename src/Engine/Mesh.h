#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <memory>
#include <glm/glm.hpp>


class Mesh {
public:
    Mesh();
    ~Mesh();

private:
    std::vector<glm::vec3> m_Vertices;
};
