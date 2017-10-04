#include "particlegenerator.h"

ParticleGenerator::ParticleGenerator()
{
    position = glm::vec3(0.0f);
    velocity = 0.00001;
    geoDescription.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    geoDescription.radius = 1.0f;
    ttl = 60;
}

void ParticleGenerator::setPosition(glm::vec3 p)
{
    position = p;
}

void ParticleGenerator::setVelocity(float v)
{
    velocity = v;
}

void ParticleGenerator::setGeometry(float r, glm::vec3 n)
{
    geoDescription.radius = r;
    geoDescription.normal = n;
}

void ParticleGenerator::setTTL(double t)
{
    ttl = t;
}

void ParticleGenerator::createParticle(ParticleObject* part)
{
    glm::vec3 discUnit = glm::rotate(geoDescription.normal, (float)PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
    float discDir = 2 * PI * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    discUnit = glm::rotate(discUnit, discDir, geoDescription.normal);
    float discLen = (1 - geoDescription.radius * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) * geoDescription.radius;
    discUnit = discUnit * discLen;
    part->setPosition(position + discUnit);
    part->setVelocity(geoDescription.normal * velocity);
    part->setTTL(ttl);
}
