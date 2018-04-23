#include "sphmodel.h"
#include "particle.h"

SPHModel::SPHModel()
{
    width = 0;
    height = 0;
    initColor = glm::vec3(1.0f);
}

SPHModel::SPHModel(int w, int h, int parts, float g, float f, float r)
{
    width = w;
    height = h;
    addParts(parts);
    initColor = glm::vec3(1.0f);
    gravity = g;
    wFriction = f;
    radius = r;
    Pconst = 1000.0;
    Dconst = 1000.0;
    Rho = 1;
    Vconst = 10.0;
    Epsilon = 10;
    grid.setCellSize(radius * 2);
    grid.setMin(glm::vec2(0.0f));
    grid.setMax(glm::vec2(width, height));
}

void SPHModel::init(int w, int h, int parts, float g, float f, float r)
{
    width = w;
    height = h;
    addParts(parts);
    initColor = glm::vec3(1.0f);
    gravity = g;
    wFriction = f;
    radius = r;
    Pconst = 300.0;
    Dconst = 20;
    Rho = 3;
    Vconst = 100;
    Epsilon = 10;
    grid.setCellSize(radius * 2);
    grid.setMin(glm::vec2(0.0f));
    grid.setMax(glm::vec2(width, height));
}

void SPHModel::update(float timeStep)
{
    grid.buildGrid(particles);
//    grid.printGridStatus();
    calcDensities();
    if(sph_state && ~SIXES) eulerTS(timeStep);
    else leapFrogTS(timeStep);
    //sixesTS(timeStep);
    enforceBounds();
}

void SPHModel::eulerTS(float timeStep)
{
    forces(timeStep);
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].velocity += particles[i].acceleration * timeStep;
        particles[i].position += particles[i].velocity * timeStep;
    }
}

void SPHModel::sixesTS(float timeStep)
{
    float a = 1/(4 - pow(4, (1/3)));
    float b = 1 - 4 * a;
    leapFrogTS(a * timeStep);
    leapFrogTS(a * timeStep);
    leapFrogTS(b * timeStep);
    leapFrogTS(a * timeStep);
    leapFrogTS(a * timeStep);
}

void SPHModel::leapFrogTS(float timeStep)
{
    for(int i = 0; i < particles.size(); i++)
    {
        glm::vec2 hold = particles[i].position;
        particles[i].position = particles[i].position + (timeStep / 2) * particles[i].velocity;
        particles[i].halfPos = hold;
    }
    forces(timeStep);
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].velocity = particles[i].velocity + particles[i].acceleration * timeStep;
    }
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].position = particles[i].halfPos + (timeStep) * particles[i].velocity;
    }
}

void SPHModel::forces(float timeStep)
{
#pragma omp parallel for
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].acceleration = glm::vec2(0.0f);
    }
    pvF(timeStep);
    if(sph_state && SFFORCES) sFrameF(timeStep);
    #pragma omp parallel for
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].acceleration /= particles[i].mass;
    }
    gravityF(timeStep);
}

void SPHModel::pvF(float timeStep)
{
    for(int i = 0; i < particles.size(); i++)
    {
        glm::vec2 holdAccel = glm::vec2(0.0f);
        std::vector<int> gpos = particles[i].getGridPos();
        for(int x = -2; x < 2; x++)
        {
            for(int y = -2; y < 2; y++)
            {
                std::vector<Particle*>* hold = grid.getPartsAt(gpos[0] + x, gpos[1] + y);
                if(hold != NULL)
                {
                    for(int j = 0; j < hold->size(); j++)
                    {
                        //holdAccel += particles[j].mass * (calcPressElement(&particles[i], &particles[j])) * h2;
                        holdAccel += (*hold)[j]->mass * (calcPressElement(&particles[i], (*hold)[j]) + calcViscElement(&particles[i], (*hold)[j])) * (*hold)[j]->findGradWeight(particles[i].position);
                    }
                    particles[i].acceleration = -holdAccel;
                }
            }
        }
    }
}

void SPHModel::sFrameF(float timeStep)
{
    for(int i = 0; i < particles.size(); i++)
    {
        Particle* cpart = particles[i].getParentPtr()->getParticle(particles[i].getSolidPtr());
        if(cpart != NULL)
        {
            float elasticity = cpart->parentPtr->getElasticity();
            glm::vec2 dVec = cpart->getPosition() - particles[i].getPosition();
            float dist = glm::length(dVec);
            dVec = glm::normalize(dVec);
            particles[i].acceleration += dVec * dist * dist * elasticity;
        }
    }
}

void SPHModel::gravityF(float timeStep)
{
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].acceleration += glm::vec2(0.0f, -1.0f) * gravity;
    }
}

void SPHModel::calcDensities()
{
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].density = 0;
        std::vector<int> gpos = particles[i].getGridPos();
        for(int x = -2; x < 2; x++)
        {
            for(int y = -2; y < 2; y++)
            {
                std::vector<Particle*>* hold = grid.getPartsAt(gpos[0] + x, gpos[1] + y);
                if(hold != NULL)
                {
                    for(int j = 0; j < hold->size(); j++)
                    {
                        particles[i].density += (*hold)[j]->mass * (*hold)[j]->findWeight(particles[i].position);
                    }
                }
            }
        }
        /*for(int j = 0; j < particles.size(); j++)
        {
            particles[i].density += particles[j].mass * particles[j].findWeight(particles[i].position);
        }*/
        particles[i].pressure = Pconst * (pow((particles[i].density / Dconst), Rho) - 1);
    }
}

float SPHModel::calcPressElement(Particle *a, Particle *b)
{
    if(a->density == 0 && b->density == 0) return 0;
    else if(a->density == 0) return (b->pressure / b->density * b->density);
    else if(b->density == 0) return (a->pressure / a->density * a->density);
    return (a->pressure / (a->density * a->density)) + (b->pressure / (b->density * b->density));
}

float SPHModel::calcViscElement(Particle *a, Particle *b)
{
    glm::vec2 distVec = a->position - b->position;
    glm::vec2 velDiffVec = a->velocity - b->velocity;
    float vr = glm::dot(distVec, velDiffVec);
    if(vr < 0)
    {
        float V;
        if(a->density != 0) V = (Vconst * a->radius) / (a->density + b->density);
        else V = 0;
        float dist = glm::length(distVec);
        return -V * (vr / (dist * dist + Epsilon * a->radius * a->radius));
    }
    else return 0;

}

void SPHModel::enforceBounds()
{
    for(int i = 0; i < particles.size(); i++)
    {
        if(particles[i].position.x < 0)
        {
            particles[i].position.x = -particles[i].position.x;
            particles[i].velocity.x = -particles[i].velocity.x * wFriction;
        }
        else if(particles[i].position.x > width)
        {
            particles[i].position.x = 2 * width - particles[i].position.x;
            particles[i].velocity.x = -particles[i].velocity.x * wFriction;
        }
        if(particles[i].position.y < 0)
        {
            particles[i].position.y = -particles[i].position.y;
            particles[i].velocity.y = -particles[i].velocity.y * wFriction;
        }
        else if(particles[i].position.y > height)
        {
            particles[i].position.y = 2 * height - particles[i].position.y;
            particles[i].velocity.y = -particles[i].velocity.y * wFriction;
        }
    }
}

void SPHModel::addParts(int parts)
{
    int sides = sqrt(parts);
    for(int i = 0; i < sides; i++)
    {
        for(int j = 0; j < sides; j++)
        {
            particles.push_back(Particle(glm::vec2(i/3.0f, j/2.0f), initColor, 1.0f, radius));
        }
    }
}

int SPHModel::addPart(Particle part)
{
    particles.push_back(part);
    return particles.size() - 1;
}

int SPHModel::addFrame(SolidFrame frame)
{
    frames.push_back(frame);
    return frames.size() - 1;
}

void SPHModel::passToDisplay(int max)
{
    int size = particles.size();
    if (size > max) size = max;
    for(int i = 0; i < size; i++)
    {
        glm::vec2 pPos = particles[i].getPosition();
        glm::vec3 pCol = particles[i].getColor();
        pointDispBuf[(i * 3)] = 2 * (pPos.x / (float)width) - (1.0f);
        pointDispBuf[(i * 3) + 1] = 2 * (pPos.y / (float)height) - (1.0f);
        pointDispBuf[(i * 3) + 2] = -1.0f;
        colDispBuf[(i * 3)] = pCol.r;
        colDispBuf[(i * 3) + 1] = pCol.g;
        colDispBuf[(i * 3) + 2] = pCol.b;
    }
    if(size < max)
    {
        for(int i = 0; i < frames.size(); i++)
        {
            size = frames[i].passToDisplay(size, max, width, height, pointDispBuf, colDispBuf);
        }
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

void SPHModel::setGravity(float g)
{
    gravity = g;
}

void SPHModel::setWFriction(float f)
{
    wFriction = f;
}

void SPHModel::setPCoefficients(float P, float D, float R)
{
    Pconst = P;
    Dconst = D;
    Rho = R;
}

void SPHModel::setPconst(float P)
{
    Pconst = P;
}

void SPHModel::setDconst(float D)
{
    Dconst = D;
}

void SPHModel::setRho(float R)
{
    Rho = R;
}

void SPHModel::setVCoefficients(float V, float E)
{
    Vconst = V;
    Epsilon = E;
}

void SPHModel::setVconst(float V)
{
    Vconst = V;
}

void SPHModel::setEpsilon(float E)
{
    Epsilon = E;
}

void SPHModel::setState(int s)
{
    sph_state = s;
}

int SPHModel::getNumParts()
{
    return particles.size();
}

int SPHModel::getHeight()
{
    return height;
}

int SPHModel::getWidth()
{
    return width;
}

int SPHModel::getState()
{
    return sph_state;
}

float SPHModel::getGravity()
{
    return gravity;
}

float SPHModel::getWFriction()
{
    return wFriction;
}

Particle* SPHModel::getPart(int i)
{
    return &particles[i];
}

SolidFrame* SPHModel::getFrame(int i)
{
    return &frames[i];
}
