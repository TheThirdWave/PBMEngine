#ifndef SOLIDFRAME_H
#define SOLIDFRAME_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include <vector>

class Particle;

class SolidFrame
{
    friend class SPHModel;
private:
    std::vector<Particle> particles;
    std::vector<glm::vec2> pVecs;
    glm::vec2 COM;
    glm::vec2 COMHalf;
    glm::vec2 COMVel;
    glm::vec2 COMAcc;
    glm::vec2 upVec;
    glm::vec2 rot;
    float mass;
    float elasticity;
public:
    SolidFrame();
    SolidFrame(float e);

    void initBuf(int size);

    int addParticle(Particle);
    void calcCOM();
    void calcRelPos();
    void updateChildren();

    void gatherForces();

    int passToDisplay(int start, int max, float width, float height, float *pBuf, float *cBuf);

    void setElasticity(float e);
    void setParticle(Particle p, int i);

    int getNumParts();
    float getElasticity();
    Particle* getParticle(int i);

};

#endif // SOLIDFRAME_H
