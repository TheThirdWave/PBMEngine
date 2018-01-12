#include "particlegenerator.h"

ParticleGenerator::ParticleGenerator()
{
    position = glm::vec3(0.0f);
    velocity = 0.00001;
    geoDescription.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    geoDescription.radius = 1.0f;
    ttl = 60;
    partsMade = 1;
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

void ParticleGenerator::setpartsMade(int p)
{
    partsMade = p;
}

void ParticleGenerator::createParticle(ParticleObject* part)
{
    if(partsMade > 0)
    {
        glm::vec3 discUnit = glm::rotate(geoDescription.normal, (float)PI/2, glm::vec3(0.0f, 1.0f, 0.0f));
        float discDir = 2 * PI * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
        discUnit = glm::rotate(discUnit, discDir, geoDescription.normal);
        float discLen = (1 - geoDescription.radius * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) * geoDescription.radius;
        discUnit = discUnit * discLen;
        glm::vec3 rVelocity = glm::cross(geoDescription.normal, glm::vec3(0.0f, 1.0f, 0.0f));
        if(glm::length(rVelocity) == 0) rVelocity = glm::cross(geoDescription.normal, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::vec3 r2Velocity = glm::cross(geoDescription.normal, rVelocity);
        rVelocity = rVelocity * (velocity * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - velocity / 2);
        r2Velocity = r2Velocity * (velocity * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - velocity / 2);
        rVelocity = geoDescription.normal + rVelocity + r2Velocity;
        part->setPosition(position + discUnit);
        part->setVelocity(rVelocity * velocity);
        part->setNewPosition(position + discUnit);
        part->setNewVelocity(geoDescription.normal * velocity);
        part->setTTL(ttl * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
        partsMade--;
    }
}
