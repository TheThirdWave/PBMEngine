#ifndef PARTICLE_H
#define PARTICLE_H

#include <../glm-0.9.8.5/glm/glm.hpp>

class Particle
{
private:
    glm::vec3 position;
    glm::vec3 color;
    float mass;
public:
    Particle();
    Particle(glm::vec3 p, glm::vec3 c, float m);
    void init(glm::vec3 p, glm::vec3 c, float m);
    void setPosition(glm::vec3 p);
    void setColor(glm::vec3 c);
    void setMass(float m);

    glm::vec3 getPosition();
    glm::vec3 getColor();
    float getMass();
};

#endif // PARTICLE_H
