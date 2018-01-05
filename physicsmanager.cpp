#include "physicsmanager.h"

PhysicsManager::PhysicsManager()
{
    generators = new ParticleGenerator*[MAX_GENERATORS];
    scaGFLen = 0;
    dirGFLen = 0;
    attGFLen = 0;
    objLen = 0;
    genLen = 0;
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

void PhysicsManager::addParticleList(ParticleObject* p, int len)
{
    partList = p;
    partLen = len;
}

void PhysicsManager::addParticleGen(ParticleGenerator * pG)
{
    generators[genLen++] = pG;
}

/*
void PhysicsManager::runTimeStep(float ts)
{
    //forces
    getAccels();

    addIntegralToNS(ts);

    float timeLeft = ts;
    //calculate next state
    for(int i = 0; i < objLen; i++)
    {
        if(objList[i]->active && objList[i]->alive)
        {
            objList[i]->getNextState(ts);
        }
    } 

    //check for collisions for objects
    for(int i = 0; i < objLen-1; i++)
    {
        if(objList[i]->alive)
        {
            for(int j = i + 1; j < objLen; j++)
            {
                if(objList[j]->alive)
                {
                    while(timeLeft > 0)
                    {
                        timeLeft = detectCollision(i, j, timeLeft);
                    }
                    timeLeft = ts;
                }
            }
        }
    }
    //check for collisions for particles
    for(int i = 0; i < partLen; i++)
    {
        for(int j = 0; j < objLen; j++)
        {
            while(timeLeft > 0)
            {
                timeLeft = detectCollision(&partList[i], objList[j], timeLeft);

            }
            timeLeft = ts;
        }
    }

    //update state
    for(int i = 0; i< objLen; i++)
    {
        objList[i]->updateState();
        objList[i]->updateRenderObject();
        objList[i]->ttl -= ts;
    }
    //update state for particles.
    for(int i = 0; i< partLen; i++)
    {
        partList[i].updateState();
        partList[i].updateRenderObject();
        partList[i].ttl -= ts;
    }
    //reset generators.
    for(int i = 0; i < genLen; i++)
    {
        generators[i]->setpartsMade(100);
    }
}
*/

void PhysicsManager::runRK4TimeStep(float ts)
{
    float timeLeft = ts;
    while(ts > 0)
    {
        getNextRK4(ts);

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

        //check for collisions for particles
/*        for(int i = 0; i < partLen; i++)
        {
            for(int j = 0; j < objLen; j++)
            {
                while(timeLeft > 0)
                {
                    timeLeft = detectCollision(&partList[i], objList[j], timeLeft);

                }
                timeLeft = ts;
            }
        }
*/
        if(ts == timeLeft) ts = 0;
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
            generators[i]->setpartsMade(100);
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

void PhysicsManager::getNextRK4(float ts)
{
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

        //apply global forces to objects
        if(attribs[i].id != EDGE || attribs[i].id != POLYGON)
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
        else if(attribs[i].id == EDGE)
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

void PhysicsManager::addIntegralToNS(float ts, int idx)
{
    for(int i = 0; i < objLen; i++)
    {
        nextState[i].acceleration = derivStates[idx][i].acceleration;
        nextState[i].velocity += derivStates[idx][i].acceleration * ts;
        nextState[i].position += derivStates[idx][i].velocity * ts;
    }
}

void PhysicsManager::combineRK4DerivStates(float ts)
{
    for(int i = 0; i < objLen; i++)
    {
        nextState[i].acceleration = (1/6.0f) * (derivStates[0][i].acceleration + 2.0f * derivStates[1][i].acceleration + 2.0f * derivStates[2][i].acceleration + derivStates[3][i].acceleration);
        nextState[i].velocity = curState[i].velocity + (ts/6.0f) * (derivStates[0][i].acceleration + 2.0f * derivStates[1][i].acceleration + 2.0f * derivStates[2][i].acceleration + derivStates[3][i].acceleration);
        nextState[i].position = curState[i].position + (ts/6.0f) * (derivStates[0][i].velocity + 2.0f * derivStates[1][i].velocity + 2.0f * derivStates[2][i].velocity + derivStates[3][i].velocity);
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
    glm::vec3 planeUp = attribs[polIdx].geo.upVec;

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
    float start = 0;
    float end = timeLeft;
    float middle = start + timeLeft / 2;
    bool collision = false;
    f = middle;
    getNextRK4(middle);
    for(int i = 0; i < sPrecision; i++)
    {
        nextSpherePos = nextState[parIdx].position;
        planeNorm = attribs[polIdx].geo.normal;
        planeUp = attribs[polIdx].geo.upVec;

        //get the position of the sphere in relation to some arbitrary point on the plane.
        srefPlane = spherePos - planePos;
        nSRefPlane = nextSpherePos - planePos;

        //use that to get the distance of the sphere from the plane by taking the dot product of the normal and the position of the sphere in terms of the point on the plane.
        origDist = glm::dot(srefPlane, planeNorm);
        newDist = glm::dot(nSRefPlane, planeNorm);

        if((origDist >= 0 && newDist >= 0) || (origDist <= 0 && newDist <= 0))
        {
            start = middle;
            middle = start + (end - start)/2;
        }
        else
        {
            end = middle;
            middle = start + (end - start)/2;
            collision = true;
        }
        f = middle;
        getNextRK4(middle);
    }

    timeLeft = timeLeft * (1-f);
    if(collision == false) return timeLeft;

    glm::vec3 point = nextSpherePos - spherePos;
    float len = std::abs(glm::dot((point), planeNorm));
    len = glm::dot((spherePos - planePos), planeNorm) / len;
    getNextRK4(timeLeft * len);
    nextSpherePos = spherePos + (point * len);
    nextSpherePos = nextState[polIdx].position;


    //get the points from the children of the polygon to use as collision edges.
    int childs[MAX_POLYGON_CHILDREN];
    int count = pol->getVertices(childs);

    //Get the barycentric coordinates of the point of collision on the plane. (We're assuming that the "polygon" is a triangle here)
    glm::vec3 e01 = curState[pol->childPtrs[childs[1]]->index].position - curState[pol->childPtrs[childs[0]]->index].position;
    glm::vec3 e12 = curState[pol->childPtrs[childs[2]]->index].position - curState[pol->childPtrs[childs[1]]->index].position;
    glm::vec3 e20 = curState[pol->childPtrs[childs[0]]->index].position - curState[pol->childPtrs[childs[2]]->index].position;
    glm::vec3 p1x = nextSpherePos - curState[pol->childPtrs[childs[1]]->index].position;
    glm::vec3 p2x = nextSpherePos - curState[pol->childPtrs[childs[2]]->index].position;
    glm::vec3 Vn = glm::cross(e01, e12);
    float area2 = glm::length(Vn);
    glm::vec3 normal = Vn/area2;
    float u = glm::dot(glm::cross(e12, p1x), normal) / area2;
    float v = glm::dot(glm::cross(e20, p2x), normal) / area2;
    float w = 1 - u - v;

    //If the barycentric coordinates are all between 0 and 1, the collision point is inside the triangle, otherwise it's a miss.
    if(u < 0 || v < 0 || w < 0) return timeLeft;

    /*//We assume edges between successive child points, so there's a line between child 0 and child 1,
    //a line between child 1 and child 2, a line between child 3 and child 4, and so on.  We also assume a line between the last child and the first, so if there
    //are n children there's a line between child n and child 0.
    //We use the children to check to see if the position of the colliding object on the collision plane is within the bounds of the polygon.
    //To do that, we discard one of the coordinate components and check the position of the colliding object against the line segements defined by the children of the polygon.
    //To avoid a degenerative case, we make sure that we're discarding the largest component of the polygon's surface normal.
    float m = std::max(std::abs(planeNorm.x), std::abs(planeNorm.y));
    m = std::max(m, std::abs(planeNorm.z));
    int numIntercepts = 0;
    for(int i = 0; i < count; i++)
    {
        glm::vec3 pt1, pt2;
        if(i < count - 1)
        {
            pt1 = pla->childPtrs[childs[i]]->getPosition();
            pt2 = pla->childPtrs[childs[i+1]]->getPosition();
        }
        else
        {
            pt1 = pla->childPtrs[childs[i]]->getPosition();
            pt2 = pla->childPtrs[childs[0]]->getPosition();
        }
        if(m == std::abs(planeNorm.x))
        {
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().y, par->getPosition().z), glm::vec2(pt1.y, pt1.z), glm::vec2(pt2.y, pt2.z));
        }
        else if(m == std::abs(planeNorm.y))
        {
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().x, par->getPosition().z), glm::vec2(pt1.x, pt1.z), glm::vec2(pt2.x, pt2.z));
        }
        else
        {
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().b, par->getPosition().y), glm::vec2(pt1.b, pt1.y), glm::vec2(pt2.b, pt2.y));
        }
    }

    if(numIntercepts % 2 == 0) return timeLeft;*/

    //get the total mass of the object hit, we give different weights to each of the vertices of the object depending on how far away they are from
    //the point of collision.
    if(attribs[polIdx].active)
    {
        float addedWeights = 0;
        float* weights = new float[pol->numChildren];
        float mass = 0;
        float cWeights = 0;
        glm::vec3 velCollPoly = glm::vec3(0.0f);
        for(int i = 0; i < pol->numChildren; i++)
        {
            addedWeights += glm::length(curState[par->index].position - curState[pol->childPtrs[i]->index].position);
        }
        for(int i = 0; i < pol->numChildren; i++)
        {
            weights[i] = 1 - (glm::length(curState[par->index].position - curState[pol->childPtrs[i]->index].position) / addedWeights);
            cWeights += weights[i] * weights[i];
            mass += weights[i] * attribs[pol->childPtrs[i]->index].mass;
            velCollPoly += weights[i] * curState[pol->childPtrs[i]->index].velocity;
        }

        float totalMass = mass / cWeights;

        //to get the center of momentum we add the moments of inertia of both objects (the face and the vector) and divide by the masses.

        glm::vec3 com = (attribs[par->index].mass * curState[par->index].velocity + totalMass * velCollPoly) / (attribs[par->index].mass + totalMass);

        //then we put the velocities of the colliding objects in terms of the center of mass.
        glm::vec3 deltaV1 = velCollPoly - com;
        glm::vec3 deltaV2 = (par->getVelocity()) - com;
        //we get the normal vectors and tangent vectors.
        glm::vec3 normV1 = glm::dot(deltaV1, planeNorm) * planeNorm;
        glm::vec3 tanV1 = deltaV1 - normV1;
        glm::vec3 normV2 = glm::dot(deltaV2, planeNorm) * planeNorm;
        glm::vec3 tanV2 = deltaV2 - normV2;
        //get the new velocities in terms of center of mass, accounting for elasticity and friction.
        normV1 = -elasticity * normV1;
        tanV1 = fcoefficient * tanV1;
        normV2 = -elasticity * normV2;
        tanV2 = fcoefficient * tanV2;
        deltaV1 = normV1 + tanV1;
        deltaV2 = normV2 + tanV2;
        deltaV2 = deltaV2 + com;

        par->setVelocity(deltaV2);
        par->getNextState(timeLeft);
        for(int i = 0; i < pol->numChildren; i++)
        {
            pol->childPtrs[i]->setVelocity((deltaV1 + com) * weights[i]);
            pol->childPtrs[i]->getNextState(timeLeft);
        }
        pol->getNextState(timeLeft);
    }
    else
    {
        glm::vec3 deltaV2 = (curState[parIdx].velocity);
        glm::vec3 normV2 = glm::dot(deltaV2, planeNorm) * planeNorm;
        glm::vec3 tanV2 = deltaV2 - normV2;
        normV2 = -elasticity * normV2;
        tanV2 = fcoefficient * tanV2;
        deltaV2 = normV2 + tanV2;
        par->setVelocity(deltaV2);
        par->getNextState(timeLeft);
    }
/*
    glm::vec3 cross = glm::dot(par->getVelocity(), planeNorm) * planeNorm;
    glm::vec3 up = -elasticity * cross;
    glm::vec3 accross = par->getVelocity() - cross;

    if(glm::length(accross) != 0)
    {
        accross = accross - std::min(fcoefficient * par->mass * glm::length(up), glm::length(accross)) * glm::normalize(accross);
    }

    glm::vec3 fnVec = up + accross;

    par->setVelocity(fnVec);
    par->getNextState(timeLeft);
    float newnewDist = (planeNorm.x * nextSpherePos.x) + (planeNorm.y * nextSpherePos.y) + (planeNorm.z * nextSpherePos.z) + -(glm::dot(planePos, planeNorm));
    if((newnewDist > 0 && newDist > 0) || (newnewDist < 0 && newDist < 0))
    {
        par->getNewPosition() = par->getNewPosition() + (planeNorm * - newnewDist);
    }

*/
    return timeLeft;//timeLeft;

}

float PhysicsManager::edgeEdge(int eo1Idx, int eo2Idx, float timeLeft)
{
    EdgeObject* eo1 = (EdgeObject*)objList[eo1Idx];
    EdgeObject* eo2 = (EdgeObject*)objList[eo2Idx];
    if(eo1->checkCollections((PhysicsObject*)eo2)) return timeLeft;

    //points for each edge
    glm::vec3 pt1 = eo1->childPtrs[0]->getPosition();
    glm::vec3 pt2 = eo2->childPtrs[0]->getPosition();
    //vectors for the line that describes the edge.
    glm::vec3 e1 = eo1->childPtrs[1]->getPosition() - pt1;
    glm::vec3 e2 = eo2->childPtrs[1]->getPosition() - pt2;
    glm::vec3 hold;
    glm::vec3 hold1;
    //The normal that describes a plane between the two edges.
    if(glm::normalize(e1) == glm::normalize(-e2)) return timeLeft;
    if(e1 == e2 || glm::cross(e1, e2) == glm::vec3(0.0f))
    {
        hold = glm::cross(pt1 - pt2, e1);
        if(glm::length(hold) == 0) hold = glm::vec3(0.0f, 1.0f, 0.0f);
        e1 += glm::normalize(hold) * 0.0000001f;
    }
    //glm::vec3 blah = glm::cross(e1, e2);
    //glm::vec3 glah = glm::normalize(blah);
    hold1 = glm::cross(e1, e2);
    if(glm::length(hold1) == 0) hold1 = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 plane = glm::normalize(hold1);

    //do the same thing with for the new positions of the edges.
    glm::vec3 npt1 = eo1->childPtrs[0]->getNewPosition();
    glm::vec3 npt2 = eo2->childPtrs[0]->getNewPosition();

    glm::vec3 ne1 = eo1->childPtrs[1]->getNewPosition() - pt1;
    glm::vec3 ne2 = eo2->childPtrs[1]->getNewPosition() - pt2;

    //get the old and new vector between the edges.
    glm::vec3 dist = pt2 - pt1;
    glm::vec3 ndist = npt2-npt1;

    //if the dot product changes sign between the old and new distance, we know that the lines have crossed the plane between them,
    //and we need to check if they actually hit or if they're miles away from each other.
    float norm = glm::dot(dist, plane);
    float nnorm = glm::dot(ndist, plane);

    //return if they don't pass through the plane
    if((norm >= 0 && nnorm >= 0) || (norm <= 0 && nnorm <= 0)) return timeLeft;

    //update edges to the point where they were at the plane (I think)
    float f;
    if(norm-nnorm != 0) f = std::abs(norm/(norm-nnorm));
    else f = 0;

    float timePassed = timeLeft * f;
    timeLeft = timeLeft * (1-f);
    //I don't think I need to update the accelerations, test without after I get it nominally working.
    //getAccelsRK4(timePassed);
    combineRK4DerivStates(timePassed);

    //get lines at the updated positions
    pt1 = eo1->childPtrs[0]->getNewPosition();
    pt2 = eo2->childPtrs[0]->getNewPosition();
    e1 = eo1->childPtrs[1]->getNewPosition() - pt1;
    e2 = eo2->childPtrs[1]->getNewPosition() - pt2;

    dist = pt2 - pt1;

    //find whether the lines intersect inside the marked segments
    float s = (glm::dot(dist, glm::cross(glm::normalize(e2), plane)))/(glm::dot(e1, glm::cross(glm::normalize(e2), plane)));
    float t = -(glm::dot(dist, glm::cross(glm::normalize(e1), plane)))/(glm::dot(e2, glm::cross(glm::normalize(e1), plane)));

    //if they meet outside the segments just return.
    if(s <= 0 || s >= 1 || t <= 0 || t >= 1) return timeLeft;


    //HACK ALERT: I don't think I take elasticity into account when calculating the velocities.
    //HACK ALERT: Also I'm not taking mass into account.
    glm::vec3 velColl1 = s*eo1->childPtrs[0]->getVelocity() + (1-s)*eo1->childPtrs[1]->getVelocity();
    glm::vec3 velColl2 = t*eo2->childPtrs[0]->getVelocity() + (1-t)*eo2->childPtrs[1]->getVelocity();
    glm::vec3 deltaV1 = velColl1 - velColl2;
    glm::vec3 deltaV2 = velColl2 - velColl1;
    glm::vec3 pDeltaV1 = deltaV2/(s * s + (1-s) * (1-s));
    glm::vec3 pDeltaV2 = deltaV1/(t * t + (1-t) * (1-t));
    eo1->childPtrs[0]->addVelocity((1-s) * pDeltaV1);
    eo1->childPtrs[1]->addVelocity((s) * pDeltaV1);
    eo2->childPtrs[0]->addVelocity((1-t) * pDeltaV2);
    eo2->childPtrs[1]->addVelocity((t) * pDeltaV2);

    eo1->getNextChildStates(timeLeft);
    eo2->getNextChildStates(timeLeft);
    eo1->getNextState(timeLeft);
    eo2->getNextState(timeLeft);

    return timeLeft;

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



































