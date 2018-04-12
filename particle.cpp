#include "particle.h"

Particle::Particle()
{
    position = glm::vec3(0.0f);
    color = glm::vec3(1.0f);
    mass = 0;
}

Particle::Particle(glm::vec3 p, glm::vec3 c, float m)
{
    position = p;
    color = c;
    mass = m;
}

void Particle::init(glm::vec3 p, glm::vec3 c, float m)
{
    position = p;
    color = c;
    mass = m;
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
