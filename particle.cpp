#include "particle.h"

Particle::Particle()
{
    position = glm::vec2(0.0f);
    halfPos = position;
    velocity = glm::vec2(0.0f);
    acceleration = glm::vec2(0.0f);
    color = glm::vec3(1.0f);
    mass = 0;
    pressure = 0;
    density = 0;
    radius = 1;
}

Particle::Particle(glm::vec2 p, glm::vec3 c, float m, float r)
{
    position = p;
    halfPos = p;
    velocity = glm::vec2(0.0f);
    acceleration = glm::vec2(0.0f);
    color = c;
    mass = m;
    pressure = 0;
    density = 0;
    radius = r;
}

void Particle::init(glm::vec2 p, glm::vec3 c, float m, float r)
{
    position = p;
    halfPos = p;
    velocity = glm::vec2(0.0f);
    acceleration = glm::vec2(0.0f);
    color = c;
    mass = m;
    pressure = 0;
    density = 0;
    radius = r;
}

float Particle::findWeight(glm::vec2 pt)
{
    float dist = glm::length(position - pt);
    if(dist > radius) return 0;
    return  pow((1 - (dist / radius)), 3) * (10 / (PI * radius * radius));
}

glm::vec2 Particle::findGradWeight(glm::vec2 pt)
{
    glm::vec2 rVec = pt - position;
    float dist = glm::length(rVec);
    if(dist > radius || dist == 0) return glm::vec2(0.0f);
    return (float)(-pow((1 - (dist / radius)), 2) * (30 / (PI * radius * radius * radius))) * (rVec / dist);;
}

void Particle::setPosition(glm::vec2 p)
{
    position = p;
}

void Particle::setHalfPos(glm::vec2 hp)
{
    halfPos = hp;
}

void Particle::setVelocity(glm::vec2 v)
{
    velocity = v;
}

void Particle::setAcceleration(glm::vec2 a)
{
    acceleration = a;
}

void Particle::setColor(glm::vec3 c)
{
    color = c;
}

void Particle::setMass(float m)
{
    mass = m;
}

void Particle::setDensity(float d)
{
    density = d;
}

void Particle::setPressure(float p)
{
    pressure = p;
}

void Particle::setRadius(float r)
{
    radius = r;
}

glm::vec2 Particle::getPosition()
{
    return position;
}

glm::vec2 Particle::getHalfPos()
{
    return halfPos;
}

glm::vec2 Particle::getVelocity()
{
    return velocity;
}

glm::vec2 Particle::getAcceleration()
{
    return acceleration;
}

glm::vec3 Particle::getColor()
{
    return color;
}

float Particle::getMass()
{
    return mass;
}

float Particle::getDensity()
{
    return density;
}

float Particle::getPressure()
{
    return pressure;
}

float Particle::getRadius()
{
    return radius;
}
