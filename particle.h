#ifndef PARTICLE_H
#define PARTICLE_H

#include <../glm-0.9.8.5/glm/glm.hpp>

class Particle
{
private:
    glm::vec3 position;
    glm::vec3 color;
    float mass;
    int dispIdx;
public:
    Particle();
    void setPosition(glm::vec3 p);
    void setColor(glm::vec3 c);
    void setMass(float m);
    void setDisplayIndex(int i);

    glm::vec3 getPosition();
    glm::vec3 getColor();
    float getMass();
    int getDisplayIndex();
};

#endif // PARTICLE_H
