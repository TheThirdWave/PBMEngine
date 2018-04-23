#ifndef SOLIDFRAME_H
#define SOLIDFRAME_H

#include <../glm-0.9.8.5/glm/glm.hpp>
#include "structpile.h"
#include <vector>

class Particle;

class SolidFrame
{
private:
    std::vector<Particle> particles;
    std::vector<glm::vec2> pVecs;
    glm::vec3 COM;
    float elasticity;
public:
    SolidFrame();
    SolidFrame(float e);
    int addParticle(Particle);
    void calcCOM();
    int passToDisplay(int start, int max, float width, float height, float *pBuf, float *cBuf);

    void setElasticity(float e);

    float getElasticity();
    Particle* getParticle(int i);

};

#endif // SOLIDFRAME_H
