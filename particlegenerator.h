#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

#include "particleobject.h"

class ParticleGenerator
{
    friend class PhysicsManager;

private:
    ParticleObject* particle;
    glm::vec3 position;
    geometry geoDescription;
    double ttl;
    float velocity;
public:
    ParticleGenerator();
    void createParticle(ParticleObject*);
    void setPosition(glm::vec3);
    void setVelocity(float);
    void setGeometry(float, glm::vec3);
    void setTTL(double);
};

#endif // PARTICLEGENERATOR_H
