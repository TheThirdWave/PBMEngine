#include "sphmodel.h"
#include "particle.h"

SPHModel::SPHModel()
{
    width = 0;
    height = 0;
    initColor = glm::vec3(1.0f);
}

SPHModel::SPHModel(int w, int h, int parts, float g, float f)
{
    width = w;
    height = h;
    addParts(parts);
    initColor = glm::vec3(1.0f);
    gravity = g;
    wFriction = f;
    Pconst = 0.000000000001;
    Dconst = 0.000000000001;
    Rho = 1;
    Vconst = 0.000001;
    Epsilon = 10000000;
}

void SPHModel::init(int w, int h, int parts, float g, float f)
{
    width = w;
    height = h;
    addParts(parts);
    initColor = glm::vec3(1.0f);
    gravity = g;
    wFriction = f;
    Pconst = 0.0001;
    Dconst = 0.0001;
    Rho = 1;
    Vconst = 10000;
    Epsilon = 10000;
}

void SPHModel::update(float timeStep)
{
    calcDensities();
    //eulerTS(timeStep);
    leapFrogTS(timeStep);
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
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].acceleration = glm::vec2(0.0f);
    }
    pvF(timeStep);
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
        for(int j = 0; j < particles.size(); j++)
        {
            if(i != j)
            {
                float h = calcPressElement(&particles[i], &particles[j]);
                float h3 =  calcViscElement(&particles[i], &particles[j]);
                glm::vec2 h2 = particles[j].findGradWeight(particles[i].position);
                if(glm::length(h2) > 0)
                {
                    int x = 1;
                }
                holdAccel += particles[j].mass * (calcPressElement(&particles[i], &particles[j])) * h2;
                //holdAccel += particles[j].mass * (calcPressElement(&particles[i], &particles[j]) + calcViscElement(&particles[i], &particles[j])) * particles[j].findGradWeight(particles[i].position);
            }
        }
        particles[i].acceleration = -holdAccel;
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
//#pragma omp parallel for
        for(int j = 0; j < particles.size(); j++)
        {
            if(i != j)
            {
                particles[i].density += particles[j].mass * particles[j].findWeight(particles[i].position);
            }
        }
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
    for(int i = 0; i < parts; i++)
    {
        particles.push_back(Particle(glm::vec2(width / 2, height / 2), initColor, 1.0f, 1.0f));
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
        glm::vec2 pPos = particles[i].getPosition();
        glm::vec3 pCol = particles[i].getColor();
        pointDispBuf[(i * 3)] = 2 * (pPos.x / (float)width) - (1.0f);
        pointDispBuf[(i * 3) + 1] = 2 * (pPos.y / (float)height) - (1.0f);
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

float SPHModel::getGravity()
{
    return gravity;
}

float SPHModel::getWFriction()
{
    return wFriction;
}
