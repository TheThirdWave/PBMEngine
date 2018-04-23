#include "solidframe.h"
#include "particle.h"

SolidFrame::SolidFrame()
{
    elasticity = 1;
}

SolidFrame::SolidFrame(float e)
{
    elasticity = e;
}

int SolidFrame::addParticle(Particle p)
{
    particles.push_back(p);
    return particles.size() - 1;
}

void SolidFrame::calcCOM()
{

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

float SolidFrame::getElasticity()
{
    return elasticity;
}

Particle* SolidFrame::getParticle(int i)
{
    return &particles[i];
}
