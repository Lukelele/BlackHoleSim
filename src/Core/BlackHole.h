#pragma once

#include "../Engine/Primitives.h"
#include "../Engine/Object.h"


class BlackHole : public Object {
public:
    BlackHole(float mass, glm::vec3 origin = glm::vec3(0,0,0));
    ~BlackHole();


private:
    float m_mass;
    float m_eventHorizonRadius;
};