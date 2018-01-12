#include "physicsmanager.h"

PhysicsManager::PhysicsManager()
{
    scaGFLen = 0;
    dirGFLen = 0;
    attGFLen = 0;
    objLen = 0;
    genLen = 0;
    genIdx = 0;
    partLen = 0;
    elasticity = 1.0f;
    fcoefficient = 0.1f;
    sPrecision = 10;
}

int PhysicsManager::addPhysObj(PhysicsObject* backPtr)
{
    if(objLen >= MAX_PHYS_OBJECTS)
    {
        fprintf(stderr,"Error, MAX_PHYS_OBJECTS reached.\n");
        return -1;
    }
    objList[objLen] = backPtr;
    memset(&curState[objLen], NULL, sizeof(state));
    memset(&nextState[objLen], NULL, sizeof(state));
    for(int i = 0; i < NUM_DERIV_STATES; i++)
    {
        memset(&derivStates[i][objLen], NULL, sizeof(state));
    }
    memset(&attribs[objLen], NULL, sizeof(attributes));
    attribs[objLen].mass = 1;
    attribs[objLen].ttl = -1;
    attribs[objLen].alive = true;
    attribs[objLen].active = true;
    attribs[objLen].solid = true;
    attribs[objLen].particle = false;
    attribs[objLen].geo.scale = glm::vec3(1.0f);
    return objLen++;
}

void PhysicsManager::addScalarForce(float f)
{
    scalarGlobalForces[scaGFLen++] = f;
}

void PhysicsManager::addDirectionalForce(glm::vec3 f)
{
    directonalGlobalForces[dirGFLen++] = f;
}

void PhysicsManager::addAttractorForce(geometry g)
{
    attractorGlobalForces[attGFLen++] = g;
}

int PhysicsManager::addParticleGen()
{
    if(genLen == 0) genIdx = 0;
    return genLen++;
}

void PhysicsManager::addParticles(int num)
{
    int hold;
    for(int i = 0; i < num; i++)
    {
        hold = addPhysObj(&particles[partLen]);
        particles[partLen].manager = this;
        particles[partLen].index = hold;
        particles[partLen].setRenderObject(vModel);
        partLen++;
        if(hold == -1 || partLen >= NUM_PARTS) break;
        attribs[hold].id = PARTICLE;
        attribs[hold].particle = true;
    }
}

ParticleGenerator* PhysicsManager::getGenerator(int idx)
{
    return &generators[idx];
}


void PhysicsManager::runTimeStep(float ts)
{
    float timeLeft = ts;
    while(ts > 0)
    {
        //We do an Euler integration to check for collisions, if we don't find any then we do a full RK4 integration.
        getNextEuler(ts);

        //check for collisions for objects
        for(int i = 0; i < objLen-1; i++)
        {
            if(attribs[i].alive && attribs[i].solid)
            {
                for(int j = i + 1; j < objLen; j++)
                {
                    if(attribs[j].alive && attribs[j].solid)
                    {
                        timeLeft = detectCollision(i, j, timeLeft);
                    }
                }
            }
        }

        if(ts == timeLeft)
        {
            ts = 0;
        }
        else ts = timeLeft;

        //update state
        for(int i = 0; i< objLen; i++)
        {
            curState[i] = nextState[i];
            if(attribs[i].id == POLYGON || attribs[i].id == EDGE || attribs[i].id == COLLECTION)
            {
                objList[i]->updateState();
            }
            objList[i]->updateRenderObject();
            if(attribs[i].particle == true && attribs[i].ttl <= 0 && genLen > 0)
            {
                attribs[i].ttl--;
                if(generators[genIdx].partsMade > 0)
                {
                    generators[genIdx].createParticle((ParticleObject*)objList[i]);
                }
                genIdx = (genIdx + 1) % genLen;
            }
        }
        //update state for particles.
        /*for(int i = 0; i< partLen; i++)
        {
            partList[i].updateState();
            partList[i].updateRenderObject();
        }*/
        //reset generators.
        for(int i = 0; i < genLen; i++)
        {
            generators[i].setpartsMade(100);
        }
    }
}


void PhysicsManager::runRK4TimeStep(float ts)
{
    float timeLeft = ts;
    while(ts > 0)
    {
        //We do an Euler integration to check for collisions, if we don't find any then we do a full RK4 integration.
        getNextEuler(ts);

        //check for collisions for objects
        for(int i = 0; i < objLen-1; i++)
        {
            if(attribs[i].alive && attribs[i].solid)
            {
                for(int j = i + 1; j < objLen; j++)
                {
                    if(attribs[j].alive && attribs[j].solid)
                    {
                        timeLeft = detectCollision(i, j, timeLeft);
                    }
                }
            }
        }

        if(ts == timeLeft)
        {
            getNextRK4(ts);
            ts = 0;
        }
        else ts = timeLeft;

        //update state
        for(int i = 0; i< objLen; i++)
        {
            curState[i] = nextState[i];
            if(attribs[i].id == POLYGON || attribs[i].id == EDGE || attribs[i].id == COLLECTION)
            {
                objList[i]->updateState();
            }
            if(attribs[i].particle == true && genLen > 0)
            {
                attribs[i].ttl--;
                if(generators[genIdx].partsMade > 0 && attribs[i].ttl <= 0)
                {
                    generators[genIdx].createParticle((ParticleObject*)objList[i]);
                }
                genIdx = (genIdx + 1) % genLen;
            }
            objList[i]->updateRenderObject();
        }
        //update state for particles.
        /*for(int i = 0; i< partLen; i++)
        {
            partList[i].updateState();
            partList[i].updateRenderObject();
        }*/
        //reset generators.
        for(int i = 0; i < genLen; i++)
        {
            generators[i].setpartsMade(100);
        }
    }
}

void PhysicsManager::setNextFromCurState()
{
    for(int i = 0; i < objLen; i++)
    {
        nextState[i] = curState[i];
    }
}

void PhysicsManager::getNextEuler(float ts)
{
    setNextFromCurState();
    getDerivFromNextState(0);
    addIntegralToNS(ts, 0);
}

void PhysicsManager::getNextRK4(float ts)
{
    setNextFromCurState();
    getDerivFromNextState(0);
    addIntegralToNS(ts / 2.0f, 0);
    getDerivFromNextState(1);
    setNextFromCurState();
    addIntegralToNS(ts / 2.0f, 1);
    getDerivFromNextState(2);
    setNextFromCurState();
    addIntegralToNS(ts, 2);
    getDerivFromNextState(3);
    combineRK4DerivStates(ts);
}

void PhysicsManager::getDerivFromNextState(int idx)
{

    //forces
    for(int i = 0; i < objLen; i++)
    {

        if(attribs[i].alive && attribs[i].active)
        {
            //apply global forces to objects
            if(attribs[i].id != EDGE && attribs[i].id != POLYGON && attribs[i].id != COLLECTION)
            {
                glm::vec3 hold = glm::vec3(0.0f);
                for(int j = 0; j < dirGFLen; j++)
                {
                    hold = hold + directonalGlobalForces[j];
                }
                for(int j = 0; j < scaGFLen; j++)
                {
                    glm::vec3 blah = nextState[i].velocity * scalarGlobalForces[j];
                    hold = hold + blah;
                }
                for(int j = 0; j < attGFLen; j++)
                {
                    glm::vec3 point = glm::normalize(attractorGlobalForces[j].normal - nextState[i].position);
                    hold = hold + (point * attractorGlobalForces[j].radius);
                }
                derivStates[idx][i].acceleration = hold / attribs[i].mass;
            }
            //spring forces.
            else if(attribs[i].id == EDGE && attribs[i].active && attribs[i].alive)
            {
                glm::vec3 pt1 = nextState[objList[i]->childPtrs[0]->getIndex()].position;
                glm::vec3 pt2 = nextState[objList[i]->childPtrs[1]->getIndex()].position;
                glm::vec3 v1 = nextState[objList[i]->childPtrs[0]->getIndex()].velocity;
                glm::vec3 v2 = nextState[objList[i]->childPtrs[1]->getIndex()].velocity;
                glm::vec3 pt12 = pt2 - pt1;
                glm::vec3 npt12 = glm::normalize(pt12);
                glm::vec3 sf = attribs[i].springK * (glm::length(pt12) - attribs[i].springL) * npt12;
                glm::vec3 df = attribs[i].springD * (glm::dot((v2 - v1), npt12)) * npt12;
                derivStates[idx][objList[i]->childPtrs[0]->index].acceleration += (sf + df) / attribs[objList[i]->childPtrs[0]->index].mass;
                derivStates[idx][objList[i]->childPtrs[1]->index].acceleration += (-sf + -df) / attribs[objList[i]->childPtrs[1]->index].mass;
            }
        }
    }

}

int PhysicsManager::getParticleList(ParticleObject* &ptr)
{
    ptr = particles;
    return partLen;
}

void PhysicsManager::addIntegralToNS(float ts, int idx)
{
    for(int i = 0; i < objLen; i++)
    {
        nextState[i].acceleration = derivStates[idx][i].acceleration;
        nextState[i].velocity += derivStates[idx][i].acceleration * ts;
        nextState[i].position += nextState[i].velocity * ts;
    }
}

void PhysicsManager::combineRK4DerivStates(float ts)
{
    for(int i = 0; i < objLen; i++)
    {
        nextState[i].acceleration = (1/6.0f) * (derivStates[0][i].acceleration + 2.0f * derivStates[1][i].acceleration + 2.0f * derivStates[2][i].acceleration + derivStates[3][i].acceleration);
        nextState[i].velocity = curState[i].velocity + (ts/6.0f) * (derivStates[0][i].acceleration + 2.0f * derivStates[1][i].acceleration + 2.0f * derivStates[2][i].acceleration + derivStates[3][i].acceleration);
        nextState[i].position = curState[i].position + (ts/6.0f) * nextState[i].velocity;
    }
}

float PhysicsManager::detectCollision(int idx1, int idx2, float timeLeft)
{
    if(attribs[idx1].id == SPHERE && attribs[idx2].id == PLANE) return spherePlane(idx1, idx2, timeLeft);
    if(attribs[idx1].id == PLANE && attribs[idx2].id == SPHERE) return spherePlane(idx2, idx1, timeLeft);
    if(attribs[idx1].id == SPHERE && attribs[idx2].id == POLYGON) return spherePoly(idx1, idx2, timeLeft);
    if(attribs[idx1].id == POLYGON && attribs[idx2].id == SPHERE) return spherePoly(idx2, idx1, timeLeft);
    if(attribs[idx1].id == PARTICLE && attribs[idx2].id == POLYGON) return partPoly(idx1, idx2, timeLeft);
    if(attribs[idx1].id == POLYGON && attribs[idx2].id == PARTICLE) return partPoly(idx2, idx1, timeLeft);
    if(attribs[idx1].id == EDGE && attribs[idx2].id == EDGE)
    {
        return edgeEdge(idx1, idx2, timeLeft);
    }
    return timeLeft;
}

float PhysicsManager::spherePlane(int sphIdx, int plaIdx, float timeLeft)
{


}

float PhysicsManager::spherePoly(int sphIdx, int polIdx, float timeLeft)
{


}

float PhysicsManager::partPoly(int parIdx, int polIdx, float timeLeft)
{
    ParticleObject* par = (ParticleObject*)objList[parIdx];
    PolygonObject* pol = (PolygonObject*)objList[polIdx];
    //check collision w/plane
    if(pol->getChildIdx((PhysicsObject*)par) >= 0) return timeLeft;
    if(pol->checkCollections((PhysicsObject*)par)) return timeLeft;
    glm::vec3 spherePos = curState[parIdx].position;
    glm::vec3 nextSpherePos = nextState[parIdx].position;
    glm::vec3 planePos = curState[polIdx].position;
    glm::vec3 planeNorm = attribs[polIdx].geo.normal;

    //get the position of the sphere in relation to some arbitrary point on the plane.
    glm::vec3 srefPlane = spherePos - planePos;
    glm::vec3 nSRefPlane = nextSpherePos - planePos;

    //use that to get the distance of the sphere from the plane by taking the dot product of the normal and the position of the sphere in terms of the point on the plane.
    float origDist = glm::dot(srefPlane, planeNorm);
    float newDist = glm::dot(nSRefPlane, planeNorm);

    //if the sign of the distance changes between the old position of the sphere and the new position of the sphere we know that it has crossed the plane and we need to do
    //more collision handling, otherwise there hasn't been a collision and we're done.
    if((origDist >= 0 && newDist >= 0) || (origDist <= 0 && newDist <= 0)) return timeLeft;

    float f;
    glm::vec3 hitPoint = nextSpherePos - spherePos;
    float len = std::abs(glm::dot((hitPoint), planeNorm));
    if(len != 0)
    {
        len = glm::dot((spherePos - planePos), planeNorm) / len;
        hitPoint = spherePos + (hitPoint * len);
        f = timeLeft * len;
    }


    //get the points from the children of the polygon to use as collision edges.
    int childs[MAX_POLYGON_CHILDREN];
    int count = pol->getVertices(childs);

    //Get the barycentric coordinates of the point of collision on the plane. (We're assuming that the "polygon" is a triangle here)
    glm::vec3 e01 = curState[pol->childPtrs[childs[1]]->index].position - curState[pol->childPtrs[childs[0]]->index].position;
    glm::vec3 e12 = curState[pol->childPtrs[childs[2]]->index].position - curState[pol->childPtrs[childs[1]]->index].position;
    glm::vec3 e20 = curState[pol->childPtrs[childs[0]]->index].position - curState[pol->childPtrs[childs[2]]->index].position;
    glm::vec3 p1x = hitPoint - curState[pol->childPtrs[childs[1]]->index].position;
    glm::vec3 p2x = hitPoint - curState[pol->childPtrs[childs[2]]->index].position;
    glm::vec3 Vn = glm::cross(e01, e12);
    float area2 = glm::length(Vn);
    glm::vec3 normal = Vn/area2;
    float u = glm::dot(glm::cross(e12, p1x), normal) / area2;
    float v = glm::dot(glm::cross(e20, p2x), normal) / area2;
    float w = 1 - u - v;

    //If the barycentric coordinates are all between 0 and 1, the collision point is inside the triangle, otherwise it's a miss.
    if(u < 0 || v < 0 || w < 0)
    {
        return timeLeft;
    }

    //get the total mass of the object hit, we give different weights to each of the vertices of the object depending on how far away they are from
    //the point of collision.
    if(attribs[polIdx].active == false)
    {
        staticPolyResponse(par, planeNorm, timeLeft);
    }
    else if(attribs[parIdx].active == false)
    {
        staticPartResponse(pol, normal, (ParticleObject*)pol->childPtrs[childs[0]], (ParticleObject*)pol->childPtrs[childs[1]], (ParticleObject*)pol->childPtrs[childs[2]], u, v, w, timeLeft);
    }
    else
    {
        partPolyResponse(par, pol, planeNorm, (ParticleObject*)pol->childPtrs[childs[0]], (ParticleObject*)pol->childPtrs[childs[1]], (ParticleObject*)pol->childPtrs[childs[2]], u, v, w, timeLeft);
    }

    return timeLeft;//timeLeft;

}

float PhysicsManager::edgeEdge(int eo1Idx, int eo2Idx, float timeLeft)
{
    EdgeObject* eo1 = (EdgeObject*)objList[eo1Idx];
    EdgeObject* eo2 = (EdgeObject*)objList[eo2Idx];
    if(eo1->checkCollections((PhysicsObject*)eo2)) return timeLeft;
    if(attribs[eo1Idx].active == false && attribs[eo1Idx].active == false) return timeLeft;

    //Check to see if the point where the lines are closest is actually on the line segments defined by the vertices.
    //points for each edge
    glm::vec3 p1 = eo1->childPtrs[0]->getPosition();
    glm::vec3 p2 = eo1->childPtrs[1]->getPosition();
    glm::vec3 q1 = eo2->childPtrs[0]->getPosition();
    glm::vec3 q2 = eo2->childPtrs[1]->getPosition();
    //vectors for the line that describes the edge.
    glm::vec3 p12 = p2 - p1;
    glm::vec3 q12 = q2 - q1;
    //get the cross product of the lines, it is the vector pointing to where the lines cross (oh now I get it)
    glm::vec3 n = glm::cross(p12, q12);
    if(n == glm::vec3(0.0f)) return timeLeft;
    n = glm::normalize(n);
    glm::vec3 r = q1 - p1;
    glm::vec3 bn = glm::cross(glm::normalize(q12), n);
    float sAll = glm::dot(p12, bn);
    float sSome = glm::dot(r, bn);
    float s = sSome/sAll;
    glm::vec3 an = glm::cross(glm::normalize(p12), n);
    float tAll = glm::dot(q12, an);
    float tSome = glm::dot(-r, an);
    float t = tSome/tAll;
    if(t >= 1 || t <= 0 || s >= 1 || s <= 0) return timeLeft;

    glm::vec3 pa = p1 + (p12 * s);
    glm::vec3 qa = q1 + (q12 * t);
    glm::vec3 m = qa - pa;

    //Do everything over again but with the next position to see if the lines actually hit each other between timesteps.
    glm::vec3 p1Next = eo1->childPtrs[0]->getNewPosition();
    glm::vec3 p2Next = eo1->childPtrs[1]->getNewPosition();
    glm::vec3 q1Next = eo2->childPtrs[0]->getNewPosition();
    glm::vec3 q2Next = eo2->childPtrs[1]->getNewPosition();
    //vectors for the line that describes the edge.
    glm::vec3 p12Next = p2Next - p1Next;
    glm::vec3 q12Next = q2Next - q1Next;
    //get the cross product of the lines, it is the vector pointing to where the lines cross (oh now I get it)
    glm::vec3 nNext = glm::cross(p12Next, q12Next);
    if(nNext == glm::vec3(0.0f)) return timeLeft;
    nNext = glm::normalize(nNext);
    glm::vec3 rNext = q1Next - p1Next;
    glm::vec3 bnNext = glm::cross(glm::normalize(q12Next), nNext);
    float sAllNext = glm::dot(p12Next, bnNext);
    float sSomeNext = glm::dot(rNext, bnNext);
    float sNext = sSomeNext/sAllNext;
    glm::vec3 anNext = glm::cross(glm::normalize(p12Next), nNext);
    float tAllNext = glm::dot(q12Next, anNext);
    float tSomeNext = glm::dot(-rNext, anNext);
    float tNext = tSomeNext/tAllNext;

    glm::vec3 paNext = p1Next + (p12Next * sNext);
    glm::vec3 qaNext = q1Next + (q12Next * tNext);
    glm::vec3 mNext = qaNext - paNext;

    if(glm::dot(m, mNext) >= 0) return timeLeft;

    if(attribs[eo2Idx].active == false)
    {
        edgeStaticResponse(eo1, (ParticleObject*)eo1->childPtrs[0], (ParticleObject*)eo1->childPtrs[1], n, pa, timeLeft);
    }
    else if(attribs[eo1Idx].active == false)
    {
        edgeStaticResponse(eo2, (ParticleObject*)eo2->childPtrs[0], (ParticleObject*)eo2->childPtrs[1], n, qa, timeLeft);
    }
    else
    {
        edgeEdgeResponse(eo1, (ParticleObject*)eo1->childPtrs[0], (ParticleObject*)eo1->childPtrs[1], pa, eo2, (ParticleObject*)eo2->childPtrs[0], (ParticleObject*)eo2->childPtrs[1], qa, n, timeLeft);
    }
}

void PhysicsManager::staticPartResponse(PolygonObject * pol, glm::vec3 normal, ParticleObject * pt1, ParticleObject * pt2, ParticleObject * pt3, float u, float v, float w, float timeLeft)
{
    glm::vec3 pt1Vel = curState[pt1->index].velocity;
    glm::vec3 pt2Vel = curState[pt2->index].velocity;
    glm::vec3 pt3Vel = curState[pt3->index].velocity;
    glm::vec3 polColVel = u * pt1Vel + v * pt2Vel + w * pt3Vel;
    glm::vec3 polColVelPar = glm::dot(polColVel, normal) * normal;
    glm::vec3 polColVelPerp = polColVel - polColVelPar;
    glm::vec3 polColVelParNew = -elasticity * polColVelPar;
    glm::vec3 polColVelPerpNew = polColVelPerp * fcoefficient;
    glm::vec3 polColVelNew = polColVelParNew + polColVelPerpNew;
    glm::vec3 polColVelDelta = polColVelNew - polColVel;
    glm::vec3 polColVelDeltaP = polColVelDelta/(u * u + v * v + w * w);

    curState[pt1->index].velocity += polColVelDeltaP * u;
    curState[pt2->index].velocity += polColVelDeltaP * v;
    curState[pt3->index].velocity += polColVelDeltaP * w;
    pol->getNextChildStates(timeLeft);
}

void PhysicsManager::staticPolyResponse(ParticleObject * part, glm::vec3 normal, float timeLeft)
{
    glm::vec3 partVel = curState[part->index].velocity;
    glm::vec3 partVelPar = glm::dot(partVel, normal) * normal;
    glm::vec3 partVelPerp = partVel - partVelPar;
    glm::vec3 partVelParNew = -elasticity * partVelPar;
    glm::vec3 partVelPerpNew = partVelPerp * fcoefficient;
    glm::vec3 partVelNew = partVelParNew + partVelPerpNew;
    curState[part->index].velocity = partVelNew;
    part->getNextState(timeLeft);
}

void PhysicsManager::partPolyResponse(ParticleObject* part, PolygonObject* pol, glm::vec3 normal, ParticleObject* pt1, ParticleObject* pt2, ParticleObject* pt3, float u, float v, float w, float timeLeft)
{
    glm::vec3 partVel = curState[part->index].velocity;
    float partMass = attribs[part->index].mass;

    glm::vec3 pt1Vel = curState[pt1->index].velocity;
    glm::vec3 pt2Vel = curState[pt2->index].velocity;
    glm::vec3 pt3Vel = curState[pt3->index].velocity;
    float pt1Mass = attribs[pt1->index].mass;
    float pt2Mass = attribs[pt2->index].mass;
    float pt3Mass = attribs[pt3->index].mass;

    float polColMass = (u * pt1Mass + v * pt2Mass + w * pt3Mass)/(u * u + v * v + w * w);
    glm::vec3 polColVel = u * pt1Vel + v * pt2Vel + w * pt3Vel;

    glm::vec3 COM = (partMass * partVel + polColMass * polColVel)/(partMass + polColMass);
    glm::vec3 partVelCOM = partVel - COM;
    glm::vec3 partVelCOMPar = glm::dot(partVelCOM, normal) * normal;
    glm::vec3 partVelCOMPerp = partVelCOM - partVelCOMPar;
    glm::vec3 partVelCOMParNew = -elasticity * partVelCOMPar;
    glm::vec3 partVelCOMPerpNew = partVelCOMPerp * fcoefficient;
    glm::vec3 partVelCOMNew = partVelCOMParNew + partVelCOMPerpNew;
    glm::vec3 partVelNew = partVelCOMNew + COM;

    glm::vec3 polColVelCOM = polColVel - COM;
    glm::vec3 polColVelCOMPar = glm::dot(polColVelCOM, normal) * normal;
    glm::vec3 polColVelCOMPerp = polColVelCOM - polColVelCOMPar;
    glm::vec3 polColVelCOMParNew = -elasticity * polColVelCOMPar;
    glm::vec3 polColVelCOMPerpNew = polColVelCOMPerp * fcoefficient;
    glm::vec3 polColVelCOMNew = polColVelCOMParNew + polColVelCOMPerpNew;
    glm::vec3 polColVelNew = polColVelCOMNew + COM;
    glm::vec3 polColDeltaVel = polColVelNew - polColVel;
    glm::vec3 polColDeltaVelP = polColDeltaVel / (u * u + v * v + w * w);

    curState[part->index].velocity = partVelNew;
    curState[pt1->index].velocity += polColDeltaVelP * u;
    curState[pt2->index].velocity += polColDeltaVelP * v;
    curState[pt3->index].velocity += polColDeltaVelP * w;
    part->getNextState(timeLeft);
    pol->getNextChildStates(timeLeft);
}

void PhysicsManager::edgeStaticResponse(EdgeObject* edge, ParticleObject* point1, ParticleObject* point2, glm::vec3 normal, glm::vec3 colPoint, float timeleft)
{
    glm::vec3 p1 = curState[point1->index].position;
    glm::vec3 p2 = curState[point2->index].position;
    glm::vec3 p1V = curState[point1->index].velocity;
    glm::vec3 p2V = curState[point2->index].velocity;
    float v = glm::length(colPoint - p1)/glm::length(p2 - p1);
    float u = glm::length(colPoint - p2)/glm::length(p2 - p1);
    glm::vec3 colVel = (p1V * u) + (p2V * v);
    glm::vec3 colVelPar = glm::dot(colVel, normal) * normal;
    glm::vec3 colVelPerp = colVel - colVelPar;
    glm::vec3 colVelParNext = -colVelPar * elasticity;
    glm::vec3 colVelPerpNext = colVelPerp * fcoefficient;
    glm::vec3 colVelNext = colVelParNext + colVelPerpNext;
    glm::vec3 deltaColVel = colVelNext - colVel;
    glm::vec3 deltaColVelPrime = deltaColVel/(v * v + u * u);
    curState[point1->index].velocity += deltaColVelPrime * u;
    curState[point2->index].velocity += deltaColVelPrime * v;
    edge->getNextChildStates(timeleft);
}

void PhysicsManager::edgeEdgeResponse(EdgeObject * edge1, ParticleObject * point11, ParticleObject * point12, glm::vec3 colPoint1, EdgeObject * edge2, ParticleObject * point21, ParticleObject * point22, glm::vec3 colPoint2, glm::vec3 normal, float timeleft)
{
    glm::vec3 p1 = curState[point11->index].position;
    glm::vec3 p2 = curState[point12->index].position;
    glm::vec3 p1V = curState[point11->index].velocity;
    glm::vec3 p2V = curState[point12->index].velocity;
    float v1 = glm::length(colPoint1 - p1)/glm::length(p2 - p1);
    float u1 = glm::length(colPoint1 - p2)/glm::length(p2 - p1);
    glm::vec3 colVel1 = (p1V * u1) + (p2V * v1);
    float colmass1 = (attribs[point11->index].mass * u1 + attribs[point12->index].mass * v1)/(u1 * u1 + v1 * v1);

    glm::vec3 q1 = curState[point21->index].position;
    glm::vec3 q2 = curState[point22->index].position;
    glm::vec3 q1V = curState[point21->index].velocity;
    glm::vec3 q2V = curState[point22->index].velocity;
    float v2 = glm::length(colPoint2 - q1)/glm::length(q2 - q1);
    float u2 = glm::length(colPoint2 - q2)/glm::length(q2 - q1);
    glm::vec3 colVel2 = (q1V * v2) + (q2V * u2);
    float colmass2 = (attribs[point21->index].mass * u2 + attribs[point22->index].mass * v2)/(u2 * u2 + v2 * v2);

    glm::vec3 COM = (colVel1 * colmass1 + colVel2 * colmass2)/(colmass1 + colmass2);
    glm::vec3 colVelCOM1 = colVel1 - COM;
    glm::vec3 colVelCOM2 = colVel2 - COM;

    glm::vec3 colVel1Par = glm::dot(colVelCOM1, normal) * normal;
    glm::vec3 colVel1Perp = colVelCOM1 - colVel1Par;
    glm::vec3 colVel1ParNext = -colVel1Par * elasticity;
    glm::vec3 colVel1PerpNext = colVel1Perp * fcoefficient;
    glm::vec3 colVel1COMNext = colVel1ParNext + colVel1PerpNext;
    glm::vec3 colVel1Next = colVel1COMNext + COM;
    glm::vec3 deltaColVel1 = colVel1Next - colVel1;
    glm::vec3 deltaColVelPrime1 = deltaColVel1 / (u1 * u1 + v1 * v1);

    glm::vec3 colVel2Par = glm::dot(colVelCOM2, normal) * normal;
    glm::vec3 colVel2Perp = colVelCOM2 - colVel2Par;
    glm::vec3 colVel2ParNext = -colVel2Par * elasticity;
    glm::vec3 colVel2PerpNext = colVel2Perp * fcoefficient;
    glm::vec3 colVel2COMNext = colVel2ParNext + colVel2PerpNext;
    glm::vec3 colVel2Next = colVel2COMNext + COM;
    glm::vec3 deltaColVel2 = colVel2Next - colVel2;
    glm::vec3 deltaColVelPrime2 = deltaColVel2 / (u2 * u2 + v2 * v2);

    curState[point11->index].velocity += deltaColVelPrime1 * u1;
    curState[point12->index].velocity += deltaColVelPrime1 * v1;
    curState[point21->index].velocity += deltaColVelPrime2 * u2;
    curState[point22->index].velocity += deltaColVelPrime2 * v2;
    edge1->getNextChildStates(timeleft);
    edge2->getNextChildStates(timeleft);
}


int PhysicsManager::pointLSeg2D(glm::vec2 pt, glm::vec2 l0, glm::vec2 l1)
{
    if(l0.x < pt.x && l1.x < pt.x) return 0;
    if(l0.y < pt.y && l1.y < pt.y) return 0;
    if(l0.y > pt.y && l1.y > pt.y) return 0;
    if(l0 == pt) return 0;
    if(l1 == pt) return 0;
    if(l0.x > pt.x && l1.x > pt.x && l0.y > pt.y && l1.y > pt.y) return 0;
    if(l0.x > pt.x && l1.x > pt.x && l0.y < pt.y && l1.y < pt.y) return 0;
    if(l0.x > pt.x && l1.x > pt.x && l0.y >= pt.y && l1.y <= pt.y)
    {
        return 1;
    }
    if(l0.x > pt.x && l1.x > pt.x && l0.y <= pt.y && l1.y >= pt.y)
    {
        return 1;
    }
    glm::vec2 lineVec = glm::normalize(l1 - l0);
    float f;
    if(lineVec.y == 0) f = 0;
    else f = (pt.y - l0.y)/lineVec.y;
    float xf = l0.x + lineVec.x * f;
    if(pt.x < xf)
    {
        return 1;
    }
    else return 0;

}

void PhysicsManager::clearAllObjects()
{
    memset(objList, NULL, sizeof(PhysicsObject*) * objLen);
    memset(attribs, NULL, sizeof(attributes) * objLen);
    for(int i = 0; i < NUM_DERIV_STATES; i++)
    {
        memset(derivStates[i], NULL, sizeof(state) * objLen);
    }
    memset(nextState, NULL, sizeof(state) * objLen);
    memset(curState, NULL, sizeof(state) * objLen);
    objLen = 0;
    memset(attractorGlobalForces, NULL, sizeof(geometry) * attGFLen);
    attGFLen = 0;
    memset(directonalGlobalForces, NULL, sizeof(glm::vec3) * dirGFLen);
    dirGFLen = 0;
    memset(scalarGlobalForces, NULL, sizeof(float) * scaGFLen);
    scaGFLen = 0;
    genLen = 0;
    partLen = 0;
}

void PhysicsManager::setParticleModel(RenderObject * ptr)
{
    vModel = ptr;
}



































