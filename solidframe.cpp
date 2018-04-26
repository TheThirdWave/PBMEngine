#include "solidframe.h"
#include "particle.h"

SolidFrame::SolidFrame()
{
    elasticity = 1;
    COMVel = glm::vec2(0.0f);
}

SolidFrame::SolidFrame(float e)
{
    elasticity = e;
    COMVel = glm::vec2(0.0f);
}

void SolidFrame::initBuf(int size)
{
    particles.resize(size);
}

int SolidFrame::addParticle(Particle p)
{
    particles.push_back(p);
    return particles.size() - 1;
}

void SolidFrame::calcCOM()
{
    COM = glm::vec2(0.0f);
    mass = 0;
    for(int i = 0; i < particles.size(); i++)
    {
        COM += particles[i].getMass() * particles[i].getPosition();
        mass += particles[i].getMass();
    }
    COM /= mass;
}

void SolidFrame::calcRelPos()
{
    for(int i = 0; i < particles.size(); i++)
    {
        pVecs.push_back(particles[i].getPosition() - COM);
    }
}

void SolidFrame::updateChildren()
{
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].setPosition(COM + pVecs[i]);
    }
}

void SolidFrame::gatherForces()
{
    COMAcc = glm::vec2(0.0f);
    for(int i = 0; i < particles.size(); i++)
    {
        COMAcc += particles[i].getMass() * particles[i].getAcceleration();
    }
    COMAcc /= mass;
}

int SolidFrame::passToDisplay(int start, int max, float width, float height, float* pBuf, float* cBuf)
{
    int size = particles.size();
    int end = start + size;
    if(start > max) return max;
    if (start + size > max) size = max;
    for(int i = start; i < end; i++)
    {
        int locIdx = i - start;
        glm::vec2 pPos = particles[locIdx].getPosition();
        glm::vec3 pCol = particles[locIdx].getColor();
        pBuf[(i * 3)] = 2 * (pPos.x / (float)width) - (1.0f);
        pBuf[(i * 3) + 1] = 2 * (pPos.y / (float)height) - (1.0f);
        pBuf[(i * 3) + 2] = -1.0f;
        cBuf[(i * 3)] = pCol.r;
        cBuf[(i * 3) + 1] = pCol.g;
        cBuf[(i * 3) + 2] = pCol.b;
    }
    return end;
}

void SolidFrame::setElasticity(float e)
{
    elasticity = e;
}

void SolidFrame::setParticle(Particle p, int i)
{
    particles[i] = p;
}

int SolidFrame::getNumParts()
{
    return particles.size();
}

float SolidFrame::getElasticity()
{
    return elasticity;
}

Particle* SolidFrame::getParticle(int i)
{
    return &particles[i];
}
