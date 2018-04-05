#include "particle.h"

Particle::Particle()
{

}

void Particle::setPosition(glm::vec3 p)
{
    position = p;
}

void Particle::setColor(glm::vec3 c)
{
    color = c;
}

void Particle::setMass(float m)
{
    mass = m;
}

void Particle::setDisplayIndex(int i)
{
    dispIdx = i;
}

glm::vec3 Particle::getPosition()
{
    return position;
}

glm::vec3 Particle::getColor()
{
    return color;
}

float Particle::getMass()
{
    return mass;
}

int Particle::getDisplayIndex()
{
    return dispIdx;
}
