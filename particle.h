#ifndef PARTICLE_H
#define PARTICLE_H

#include <../glm-0.9.8.5/glm/glm.hpp>
#include "structpile.h"
#include <vector>

class SolidFrame;

class Particle
{
    friend class SPHModel;
private:
    SolidFrame* parentPtr;
    int solidPtr;
    glm::vec2 position;
    glm::vec2 halfPos;
    glm::vec2 velocity;
    glm::vec2 acceleration;
    glm::vec3 color;
    float mass, density, pressure, radius;
    int gridX, gridY;
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
    void setGridPos(int x, int y);
    void setSolidPtr(int sp);
    void setParentPtr(SolidFrame* pp);

    glm::vec2 getPosition();
    glm::vec2 getHalfPos();
    glm::vec2 getVelocity();
    glm::vec2 getAcceleration();
    glm::vec3 getColor();
    float getMass();
    float getDensity();
    float getPressure();
    float getRadius();
    std::vector<int> getGridPos();
    int getSolidPtr();
    SolidFrame* getParentPtr();
};

#endif // PARTICLE_H
