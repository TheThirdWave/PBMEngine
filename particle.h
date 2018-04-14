#ifndef PARTICLE_H
#define PARTICLE_H

#include <../glm-0.9.8.5/glm/glm.hpp>
#include "structpile.h"

class Particle
{
    friend class SPHModel;
private:
    glm::vec2 position;
    glm::vec2 halfPos;
    glm::vec2 velocity;
    glm::vec2 acceleration;
    glm::vec3 color;
    float mass, density, pressure, radius;
public:
    Particle();
    Particle(glm::vec2 p, glm::vec3 c, float m, float r);
    void init(glm::vec2 p, glm::vec3 c, float m, float r);

    float findWeight(glm::vec2 pt);
    glm::vec2 findGradWeight(glm::vec2 pt);

    void setPosition(glm::vec2 p);
    void setVelocity(glm::vec2 v);
    void setAcceleration(glm::vec2 a);
    void setHalfPos(glm::vec2 hp);
    void setColor(glm::vec3 c);
    void setMass(float m);
    void setDensity(float d);
    void setPressure(float p);
    void setRadius(float r);

    glm::vec2 getPosition();
    glm::vec2 getHalfPos();
    glm::vec2 getVelocity();
    glm::vec2 getAcceleration();
    glm::vec3 getColor();
    float getMass();
    float getDensity();
    float getPressure();
    float getRadius();
};

#endif // PARTICLE_H
