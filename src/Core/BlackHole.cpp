#include "BlackHole.h"


BlackHole::BlackHole(float mass, glm::vec3 origin)
    : Object(new CircleFan(2 * mass, origin)), m_mass(mass), m_eventHorizonRadius(2 * mass)
{
}

BlackHole::~BlackHole()
{
}