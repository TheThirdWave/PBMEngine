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
    Epsilon = 0.1;
    grid.setCellSize(radius * 2);
    grid.setMin(glm::vec2(0.0f));
    grid.setMax(glm::vec2(width, height));
}

void SPHModel::update(float timeStep)
{

//    grid.printGridStatus();
    calcDensities();
    if(!(sph_state & SIXES)) leapFrogTS(timeStep);
    else sixesTS(timeStep);


}

void SPHModel::eulerTS(float timeStep)
{
    forces(timeStep);
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].velocity += particles[i].acceleration * timeStep;
        particles[i].position += particles[i].velocity * timeStep;
    }
    enforceBounds();
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
    //update particle position by a half-time step.
    for(int i = 0; i < particles.size(); i++)
    {
        glm::vec2 hold = particles[i].position;
        particles[i].position = particles[i].position + (timeStep / 2) * particles[i].velocity;
        particles[i].halfPos = hold;
    }
    //update frame position by a half-time step.
    for(int i = 0; i < frames.size(); i++)
    {
        glm::vec2 hold = frames[i].COM;
        frames[i].COM = frames[i].COM + (timeStep / 2) * frames[i].COMVel;
        frames[i].COMHalf = hold;
    }
    //update circle position by a half-time step.
    for(int i = 0; i < circles.size(); i++)
    {
        glm::vec2 hold = circles[i].position;
        circles[i].position = circles[i].position + (timeStep / 2) * circles[i].velocity;
        circles[i].halfPos = hold;
    }
    //enforce bounds and handle collisions since we moved positions.
    //circPartCollide(timeStep);
    enforceBounds();
    //build a new speed-up grid and calculate the forces for all objects at the half-time step.
    grid.buildGrid(particles);
    forces(timeStep);
    //update particle velocity w/new acceleration
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].velocity = particles[i].velocity + particles[i].acceleration * timeStep;
    }
    //update frame velocity w/new acceleration
    for(int i = 0; i < frames.size(); i++)
    {
        frames[i].COMVel = frames[i].COMVel + frames[i].COMAcc * timeStep;
    }
    //update circle velocity w/new acceleration
    for(int i = 0; i < circles.size(); i++)
    {
        circles[i].velocity = circles[i].velocity + circles[i].acceleration * timeStep;
    }
    //update the original particle position by the velocity by a full timestep (we're doing RK2 not leapfrog sue me)
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].position = particles[i].halfPos + (timeStep) * particles[i].velocity;
    }
    //update the original frame position by the velocity by a full timestep (we're doing RK2 not leapfrog sue me)
    for(int i = 0; i < frames.size(); i++)
    {
        frames[i].COM = frames[i].COMHalf + frames[i].COMVel * timeStep;
    }
    //update the original circle position by the velocity by a full timestep (we're doing RK2 not leapfrog sue me)
    for(int i = 0; i < circles.size(); i++)
    {
        circles[i].position = circles[i].halfPos + (timeStep) * circles[i].velocity;
    }
    //enforce bounds and handle collisions since we moved positions.
    circPartCollide(timeStep);
    enforceBounds();
    //update the positions of the child points for the frames.
    updateFrameChilds(timeStep);
}

void SPHModel::forces(float timeStep)
{
#pragma omp parallel for
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].acceleration = glm::vec2(0.0f);
    }
    pvF(timeStep);
    if(sph_state & SFFORCES)
    {
        sFrameF(timeStep);
        //circF(timeStep);
    }
    #pragma omp parallel for
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].acceleration /= particles[i].mass;
    }
    updateFrameForces(timeStep);
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
        if(particles[i].getParentPtr() != NULL)
        {
            Particle* cpart = particles[i].getParentPtr()->getParticle(particles[i].getSolidPtr());
            if(cpart != NULL)
            {
                float elasticity = cpart->parentPtr->getElasticity();
                glm::vec2 dVec = cpart->getPosition() - particles[i].getPosition();
                float dist = glm::length(dVec);
                if(dist == 0.0f) particles[i].acceleration += 0.0f;
                else
                {
                    dVec = glm::normalize(dVec);
                    particles[i].acceleration += dVec * dist * dist * elasticity;
                    cpart->setAcceleration(-particles[i].acceleration);
                }
            }
        }
    }
}

void SPHModel::gravityF(float timeStep)
{
    for(int i = 0; i < particles.size(); i++)
    {
        particles[i].acceleration += glm::vec2(0.0f, -1.0f) * gravity;
    }
    for(int i = 0; i < frames.size(); i++)
    {
        frames[i].COMAcc += glm::vec2(0.0f, -1.0f) * gravity;
    }
    for(int i = 0; i < circles.size(); i++)
    {
        circles[i].acceleration += glm::vec2(0.0f, -1.0f) * gravity;
    }
}

void SPHModel::circF(float timeStep)
{
    for(int i = 0; i < circles.size(); i++)
    {
        glm::vec2 holdAccel = glm::vec2(0.0f);
        for(int j = 0; j < particles.size(); j++)
        {
            holdAccel += particles[j].mass * calcPressElement(&particles[j], &circles[i]) * circles[i].findGradWeight(particles[j].position);
        }
        circles[i].acceleration = -holdAccel;
    }
}

void SPHModel::circPartCollide(float timeStep)
{
    //first determine if there's a collision.
    for(int i = 0; i < particles.size(); i++)
    {
        for(int j = 0; j < circles.size(); j++)
        {
            glm::vec2 distVec = particles[i].position - circles[j].position;
            float dist = glm::length(distVec);
            //if the distance between the two objects is less than their combined radii, then they're hitting each other.
            if(dist < particles[i].radius + circles[j].radius)
            {
                //if they're hitting each other, we have to do some momentum calculations.
                //the center of momentum (com) is where everything pivots around, basically.
                glm::vec2 com = ((particles[i].velocity * particles[i].mass) + (circles[j].velocity * circles[j].mass)) / (particles[i].mass * circles[j].mass);
                //then we get the velocities of the objects relative to the center of momentum.
                glm::vec2 vpartcom = particles[i].velocity - com;
                glm::vec2 vcirccom = circles[i].velocity - com;
                //we need the normals relative to each object at the collision point, since they're both essentially circles this is easy.
                glm::vec2 partnorm = -glm::normalize(distVec);
                glm::vec2 circnorm = -partnorm;
                //now we reflect their velocities around the normals and multiply it by some random elasticity factor, for now we'll use the global wall friction value.
                //first we get the normal component of the velocity by getting the dot product of the velocity and the collision normals.
                glm::vec2 vpartcomnorm = partnorm * -glm::dot(vpartcom, partnorm) * wFriction;
                glm::vec2 vcirccomnorm = circnorm * -glm::dot(vcirccom, circnorm) * wFriction;
                //we add twice the reflected normal vector to the velocity to get the reflected velocity.
                vpartcom += 2.0f * vpartcomnorm;
                vcirccom += 2.0f * vcirccomnorm;
                //finally, to get the updated velocities relative to each object, we add the center of momentum value back.
                particles[i].velocity = vpartcom + com;
                circles[j].velocity = vcirccom + com;
            }
        }
    }
}

void SPHModel::updateFrameForces(float timeStep)
{
    for(int i = 0; i < frames.size(); i++)
    {
        frames[i].gatherForces();
    }
}

void SPHModel::updateFrameChilds(float timeStep)
{
    for(int i = 0; i < frames.size(); i++)
    {
        frames[i].updateChildren();
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

int SPHModel::addCircle(Particle c)
{
    circles.push_back(c);
    return circles.size() - 1;
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

        int start = size;
        int end = start + circles.size();
        if(start > max) return;
        if (start + size > max) size = max;
        for(int i = start; i < end; i++)
        {
            int locIdx = i - start;
            glm::vec2 pPos = circles[locIdx].getPosition();
            glm::vec3 pCol = circles[locIdx].getColor();
            pointDispBuf[(i * 3)] = 2 * (pPos.x / (float)width) - (1.0f);
            pointDispBuf[(i * 3) + 1] = 2 * (pPos.y / (float)height) - (1.0f);
            pointDispBuf[(i * 3) + 2] = -1.0f;
            colDispBuf[(i * 3)] = pCol.r;
            colDispBuf[(i * 3) + 1] = pCol.g;
            colDispBuf[(i * 3) + 2] = pCol.b;
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

int SPHModel::getNumVerts()
{
    int ret = particles.size();
    for(int i = 0; i < frames.size(); i++)
    {
        ret += frames[i].getNumParts();
    }
    ret += circles.size();
    return ret;
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

float SPHModel::getRadius()
{
    return radius;
}

Particle* SPHModel::getPart(int i)
{
    return &particles[i];
}

SolidFrame* SPHModel::getFrame(int i)
{
    return &frames[i];
}

Particle* SPHModel::getCircle(int i)
{
    return &circles[i];
}
