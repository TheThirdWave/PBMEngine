#include "sphmodel.h"
#include "particle.h"

SPHModel::SPHModel()
{
    width = 0;
    height = 0;
    initColor = glm::vec3(1.0f);
}

SPHModel::SPHModel(int w, int h, int parts)
{
    width = w;
    height = h;
    addParts(parts);
    initColor = glm::vec3(1.0f);
}

void SPHModel::init(int w, int h, int parts)
{
    width = w;
    height = h;
    addParts(parts);
    initColor = glm::vec3(1.0f);
}

void SPHModel::addParts(int parts)
{
    for(int i = 0; i < parts; i++)
    {
        particles.push_back(Particle(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f));
    }
}

void SPHModel::addPart(Particle part)
{
    particles.push_back(part);
}

void SPHModel::passToDisplay(int max)
{
    int size = particles.size();
    if (size > max) size = max;
    for(int i = 0; i < size; i++)
    {
        glm::vec3 pPos = particles[i].getPosition();
        glm::vec3 pCol = particles[i].getColor();
        pointDispBuf[(i * 3)] = (pPos.x / (float)width) - (0.5f);
        pointDispBuf[(i * 3) + 1] = (pPos.y / (float)height) - (0.5f);
        pointDispBuf[(i * 3) + 2] = -1.0f;
        colDispBuf[(i * 3)] = pCol.r;
        colDispBuf[(i * 3) + 1] = pCol.g;
        colDispBuf[(i * 3) + 2] = pCol.b;
    }
}

void SPHModel::setInitColor(glm::vec3 col)
{
    initColor = col;
}

void SPHModel::setpointDispBuf(float *points)
{
    pointDispBuf = points;
}

void SPHModel::setcolDispBuf(float *cols)
{
    colDispBuf = cols;
}

int SPHModel::getNumParts()
{
    return particles.size();
}
