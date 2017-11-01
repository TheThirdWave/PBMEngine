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
}

void PhysicsManager::addPhysObj(PhysicsObject* obj)
{
    objList[objLen++] = obj;
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

void PhysicsManager::runTimeStep(float ts)
{
    //forces
    for(int i = 0; i < objLen; i++)
    {
        //apply global forces to objects
        if(objList[i]->id != EDGE)
        {
            glm::vec3 hold = glm::vec3(0.0f);
            for(int j = 0; j < dirGFLen; j++)
            {
                hold = hold + directonalGlobalForces[j];
            }
            for(int j = 0; j < scaGFLen; j++)
            {
                glm::vec3 blah = objList[i]->getVelocity() * scalarGlobalForces[j];
                hold = hold + blah;
            }
            for(int j = 0; j < attGFLen; j++)
            {
                glm::vec3 point = glm::normalize(attractorGlobalForces[j].normal - objList[i]->getPosition());
                hold = hold + (point * attractorGlobalForces[j].radius);
            }
            objList[i]->setAcceleration(hold);
        }
        //spring forces.
        else
        {
            glm::vec3 pt1 = objList[i]->childPtrs[0]->getPosition();
            glm::vec3 pt2 = objList[i]->childPtrs[1]->getPosition();
            glm::vec3 v1 = objList[i]->childPtrs[0]->getVelocity();
            glm::vec3 v2 = objList[i]->childPtrs[1]->getVelocity();
            glm::vec3 pt12 = pt2 - pt1;
            glm::vec3 npt12 = glm::normalize(pt12);
            glm::vec3 sf = objList[i]->springK * (glm::length(pt12) - objList[i]->springL) * npt12;
            glm::vec3 df = objList[i]->springD * (glm::dot((v2 - v1), npt12)) * npt12;
            objList[i]->childPtrs[0]->addAcceleration(sf + df);
            objList[i]->childPtrs[1]->addAcceleration(-sf + -df);
        }
    }

    //apply global forces for particles
    for(int i = 0; i < partLen; i++)
    {
        if(partList[i].ttl < 0)
        {
            int lucky = rand() % genLen;
            generators[lucky]->createParticle(&partList[i]);
        }
        glm::vec3 hold = glm::vec3(0.0f);
        for(int j = 0; j < dirGFLen; j++)
        {
            hold = hold + directonalGlobalForces[j];
        }
        for(int j = 0; j < scaGFLen; j++)
        {
            glm::vec3 blah = partList[i].getVelocity() * scalarGlobalForces[j];
            hold = hold + blah;
        }
        for(int j = 0; j < attGFLen; j++)
        {
            glm::vec3 point = glm::normalize(attractorGlobalForces[j].normal - partList[i].getPosition());
            hold = hold + (point * attractorGlobalForces[j].radius);
        }
        partList[i].setAcceleration(hold);
        partList[i].getNextState(ts);
    }

    float timeLeft = ts;
    //calculate next state
    for(int i = 0; i < objLen; i++)
    {
        objList[i]->getNextState(ts);
    } 

    //check for collisions for objects
    for(int i = 0; i < objLen-1; i++)
    {
        for(int j = i + 1; j < objLen; j++)
        {
            while(timeLeft > 0)
            {
                timeLeft = detectCollision(objList[i], objList[j], timeLeft);

            }
            timeLeft = ts;
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

float PhysicsManager::detectCollision(PhysicsObject* one, PhysicsObject* two, float timeLeft)
{
    if(one->id == SPHERE && two->id == PLANE) return spherePlane((SphereObject*)one, (PlaneObject*)two, timeLeft);
    if(one->id == PLANE && two->id == SPHERE) return spherePlane((SphereObject*)two, (PlaneObject*)one, timeLeft);
    if(one->id == SPHERE && two->id == POLYGON) return spherePoly((SphereObject*)one, (PolygonObject*)two, timeLeft);
    if(one->id == POLYGON && two->id == SPHERE) return spherePoly((SphereObject*)two, (PolygonObject*)one, timeLeft);
    if(one->id == PARTICLE && two->id == POLYGON) return partPoly((ParticleObject*)one, (PolygonObject*)two, timeLeft);
    if(one->id == POLYGON && two->id == PARTICLE) return partPoly((ParticleObject*)two, (PolygonObject*)one, timeLeft);
    if(one->id == EDGE && two->id == EDGE) return edgeEdge((EdgeObject*)one, (EdgeObject*)two, timeLeft);
    return 0;
}

float PhysicsManager::determineCollision(PhysicsObject* one, PhysicsObject* two, float timeLeft)
{
    if(one->id == SPHERE && two->id == PLANE) return spherePlaneDet((SphereObject*)one, (PlaneObject*)two);
    if(one->id == PLANE && two->id == SPHERE) return spherePlaneDet((SphereObject*)two, (PlaneObject*)one);
}

float PhysicsManager::spherePlane(SphereObject * sph, PlaneObject * pla, float timeLeft)
{
    //check collisions
    glm::vec3 spherePos = sph->getPosition();
    glm::vec3 nextSpherePos = sph->getNewPosition();
    float radius = sph->geoDescription.radius;
    glm::vec3 planePos = pla->getPosition();
    glm::vec3 planeNorm = pla->geoDescription.normal;

    glm::vec3 direction = nextSpherePos - spherePos;

    if(glm::dot(direction, planeNorm) > 0) planePos = planePos - (radius * planeNorm);
    else if(glm::dot(direction, planeNorm) < 0) planePos = planePos + (radius * planeNorm);

    float origDist = (planeNorm.x * spherePos.x) + (planeNorm.y * spherePos.y) + (planeNorm.z * spherePos.z) + -(glm::dot(planePos, planeNorm));
    float newDist = (planeNorm.x * nextSpherePos.x) + (planeNorm.y * nextSpherePos.y) + (planeNorm.z * nextSpherePos.z) + -(glm::dot(planePos, planeNorm));
    if(!(newDist < 0 && origDist > 0 || newDist > 0 && origDist < 0 || newDist == 0 && origDist == 0)) return 0;

    //determine collisions
    float f = std::abs(origDist / (origDist - newDist));

    //handle collision
    sph->getNextState(timeLeft * f);
    sph->updateState();

    glm::vec3 cross = glm::dot(sph->getVelocity(), planeNorm) * planeNorm;
    glm::vec3 up = -elasticity * cross;
    glm::vec3 accross = sph->getVelocity() - cross;

    if(glm::length(accross) != 0)
    {
        accross = accross - std::min(fcoefficient * sph->mass * glm::length(up), glm::length(accross)) * glm::normalize(accross);
    }
    glm::vec3 fnVec = up + accross;

    sph->setVelocity(fnVec);
    timeLeft = timeLeft * (1-f);
    sph->getNextState(timeLeft);


    return timeLeft;

}

float PhysicsManager::spherePoly(SphereObject * sph, PolygonObject * pla, float timeLeft)
{
    //check collision w/plane
    glm::vec3 spherePos = sph->getPosition();
    glm::vec3 nextSpherePos = sph->getNewPosition();
    float radius = sph->geoDescription.radius;
    glm::vec3 planePos = pla->getPosition();
    glm::vec3 planeNorm = pla->geoDescription.normal;
    glm::vec3 planeUp = pla->geoDescription.upVec;

    glm::vec3 direction = nextSpherePos - spherePos;

    if(glm::dot(direction, planeNorm) > 0) planePos = planePos - (radius * planeNorm);
    else if(glm::dot(direction, planeNorm) < 0) planePos = planePos + (radius * planeNorm);

    float origDist = (planeNorm.x * spherePos.x) + (planeNorm.y * spherePos.y) + (planeNorm.z * spherePos.z) + -(glm::dot(planePos, planeNorm));
    float newDist = (planeNorm.x * nextSpherePos.x) + (planeNorm.y * nextSpherePos.y) + (planeNorm.z * nextSpherePos.z) + -(glm::dot(planePos, planeNorm));
    if(!(newDist < 0 && origDist > 0 || newDist > 0 && origDist < 0 || newDist == 0 && origDist == 0)) return 0;


    //move sphere up to where it connects with the plane
    float f = std::abs(origDist / (origDist - newDist));


    sph->getNextState(timeLeft * f);
    sph->updateState();

    //get the verticies from the polygon and get them into world coordinates.
    int numIdx = pla->rendrPtr->data->idxLen;
    int numTris = numIdx / 3;
    unsigned int *indicies = pla->rendrPtr->data->indicies;
    float* verticies = pla->rendrPtr->data->vertices;
    glm::vec3 vecs[numIdx];
    triangle tris[numTris];

    glm::mat4 rot;
    if(glm::dot(planeNorm, planeUp) != -1) rot = glm::orientation(planeNorm, planeUp);
    else rot = glm::rotate(glm::mat4(1.0f), (float)PI, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), pla->getPosition());
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), pla->scale);
    glm::mat4 trans = pos * rot * sca;

    for(int i = 0; i < numTris; i ++)
    {
        for(int j = 3 * i; j < 3 * (i + 1); j++)
        {
            glm::vec4 h = glm::vec4(verticies[(indicies[j] * 3)], verticies[(indicies[j] * 3) + 1], verticies[(indicies[j] * 3) + 2], 1.0f);
            h = trans * h;
            vecs[j] = glm::vec3(h.x, h.y, h.z);
        }
        tris[i].a = vecs[(i * 3)];
        tris[i].b = vecs[(i * 3) + 1];
        tris[i].c = vecs[(i * 3) + 2];
    }

    //discard one of the vector components and check the polygon in 2D
    float m = std::max(std::abs(planeNorm.x), std::abs(planeNorm.y));
    m = std::max(m, std::abs(planeNorm.z));
    int numIntercepts = 0;
    for(int i = 0; i < numTris; i++)
    {
        if(m == std::abs(planeNorm.x))
        {
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().y, sph->getPosition().z), glm::vec2(tris[i].a.y, tris[i].a.z), glm::vec2(tris[i].b.y, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().y, sph->getPosition().z), glm::vec2(tris[i].c.y, tris[i].c.z), glm::vec2(tris[i].b.y, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().y, sph->getPosition().z), glm::vec2(tris[i].a.y, tris[i].a.z), glm::vec2(tris[i].c.y, tris[i].c.z));
        }
        else if(m == std::abs(planeNorm.y))
        {
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().x, sph->getPosition().z), glm::vec2(tris[i].a.x, tris[i].a.z), glm::vec2(tris[i].b.x, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().x, sph->getPosition().z), glm::vec2(tris[i].c.x, tris[i].c.z), glm::vec2(tris[i].b.x, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().x, sph->getPosition().z), glm::vec2(tris[i].a.x, tris[i].a.z), glm::vec2(tris[i].c.x, tris[i].c.z));
        }
        else
        {
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().x, sph->getPosition().y), glm::vec2(tris[i].a.x, tris[i].a.y), glm::vec2(tris[i].b.x, tris[i].b.y));
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().x, sph->getPosition().y), glm::vec2(tris[i].c.x, tris[i].c.y), glm::vec2(tris[i].b.x, tris[i].b.y));
            numIntercepts += pointLSeg2D(glm::vec2(sph->getPosition().x, sph->getPosition().y), glm::vec2(tris[i].a.x, tris[i].a.y), glm::vec2(tris[i].c.x, tris[i].c.y));
        }
    }
    timeLeft = timeLeft * (1-f);
    if(numIntercepts % 2 == 0) return timeLeft;

    glm::vec3 cross = glm::dot(sph->getVelocity(), planeNorm) * planeNorm;
    glm::vec3 up = -elasticity * cross;
    glm::vec3 accross = sph->getVelocity() - cross;

    if(glm::length(accross) != 0)
    {
        accross = accross - std::min(fcoefficient * sph->mass * glm::length(up), glm::length(accross)) * glm::normalize(accross);
    }

    glm::vec3 fnVec = up + accross;

    sph->setVelocity(fnVec);
    sph->getNextState(timeLeft);


    return timeLeft;

}

float PhysicsManager::partPoly(ParticleObject * par, PolygonObject * pla, float timeLeft)
{
    //check collision w/plane
    glm::vec3 spherePos = par->getPosition();
    glm::vec3 nextSpherePos = par->getNewPosition();
    glm::vec3 planePos = pla->getPosition();
    glm::vec3 planeNorm = pla->geoDescription.normal;
    glm::vec3 planeUp = pla->geoDescription.upVec;

    float origDist = (planeNorm.x * spherePos.x) + (planeNorm.y * spherePos.y) + (planeNorm.z * spherePos.z) + -(glm::dot(planePos, planeNorm));
    float newDist = (planeNorm.x * nextSpherePos.x) + (planeNorm.y * nextSpherePos.y) + (planeNorm.z * nextSpherePos.z) + -(glm::dot(planePos, planeNorm));
    if(!(newDist < 0 && origDist > 0 || newDist > 0 && origDist < 0 || newDist == 0 && origDist == 0)) return 0;


    //move sphere up to where it connects with the plane
    float f = std::abs(origDist / (origDist - newDist));

    par->getNextState(timeLeft * f);
    par->updateState();

    //get the verticies from the polygon and get them into world coordinates.
    int numIdx = pla->rendrPtr->data->idxLen;
    int numTris = numIdx / 3;
    unsigned int *indicies = pla->rendrPtr->data->indicies;
    float* verticies = pla->rendrPtr->data->vertices;
    glm::vec3 vecs[numIdx];
    triangle tris[numTris];

    glm::mat4 rot;
    if(glm::dot(planeNorm, planeUp) != -1) rot = glm::orientation(planeNorm, planeUp);
    else rot = glm::rotate(glm::mat4(1.0f), (float)PI, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), pla->getPosition());
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), pla->scale);
    glm::mat4 trans = pos * rot * sca;

    for(int i = 0; i < numTris; i ++)
    {
        for(int j = 3 * i; j < 3 * (i + 1); j++)
        {
            glm::vec4 h = glm::vec4(verticies[(indicies[j] * 3)], verticies[(indicies[j] * 3) + 1], verticies[(indicies[j] * 3) + 2], 1.0f);
            h = trans * h;
            vecs[j] = glm::vec3(h.x, h.y, h.z);
        }
        tris[i].a = vecs[(i * 3)];
        tris[i].b = vecs[(i * 3) + 1];
        tris[i].c = vecs[(i * 3) + 2];
    }

    //discard one of the vector components and check the polygon in 2D
    float m = std::max(std::abs(planeNorm.x), std::abs(planeNorm.y));
    m = std::max(m, std::abs(planeNorm.z));
    int numIntercepts = 0;
    for(int i = 0; i < numTris; i++)
    {
        if(m == std::abs(planeNorm.x))
        {
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().y, par->getPosition().z), glm::vec2(tris[i].a.y, tris[i].a.z), glm::vec2(tris[i].b.y, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().y, par->getPosition().z), glm::vec2(tris[i].c.y, tris[i].c.z), glm::vec2(tris[i].b.y, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().y, par->getPosition().z), glm::vec2(tris[i].a.y, tris[i].a.z), glm::vec2(tris[i].c.y, tris[i].c.z));
        }
        else if(m == std::abs(planeNorm.y))
        {
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().x, par->getPosition().z), glm::vec2(tris[i].a.x, tris[i].a.z), glm::vec2(tris[i].b.x, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().x, par->getPosition().z), glm::vec2(tris[i].c.x, tris[i].c.z), glm::vec2(tris[i].b.x, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().x, par->getPosition().z), glm::vec2(tris[i].a.x, tris[i].a.z), glm::vec2(tris[i].c.x, tris[i].c.z));
        }
        else
        {
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().x, par->getPosition().y), glm::vec2(tris[i].a.x, tris[i].a.y), glm::vec2(tris[i].b.x, tris[i].b.y));
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().x, par->getPosition().y), glm::vec2(tris[i].c.x, tris[i].c.y), glm::vec2(tris[i].b.x, tris[i].b.y));
            numIntercepts += pointLSeg2D(glm::vec2(par->getPosition().x, par->getPosition().y), glm::vec2(tris[i].a.x, tris[i].a.y), glm::vec2(tris[i].c.x, tris[i].c.y));
        }
    }
    timeLeft = timeLeft * (1-f);
    if(numIntercepts % 2 == 0) return timeLeft;

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


    return timeLeft;

}

float PhysicsManager::edgeEdge(EdgeObject * eo1, EdgeObject * eo2, float timeLeft)
{
    //points for each edge
    glm::vec3 pt1 = eo1->childPtrs[0]->getPosition();
    glm::vec3 pt2 = eo2->childPtrs[0]->getPosition();
    //vectors for the line that describes the edge.
    glm::vec3 e1 = eo1->childPtrs[1]->getPosition() - pt1;
    glm::vec3 e2 = eo2->childPtrs[1]->getPosition() - pt2;
    //The normal that describes a plane between the two edges.
    glm::vec3 plane = glm::normalize(glm::cross(e1, e2));

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
    if((norm > 0 && nnorm > 0) || norm < 0 && nnorm < 0) return 0;

    //update edges to the point where they were at the plane (I think)
    float f;
    if(norm-nnorm != 0) f = std::abs(norm/(norm-nnorm));
    else f = 0;

    eo1->getNextChildStates(timeLeft * f);
    eo2->getNextChildStates(timeLeft * f);
    eo1->updateChildren();
    eo2->updateChildren();
    eo1->getNextState(timeLeft * f);
    eo2->getNextState(timeLeft * f);
    eo1->updateState();
    eo2->updateState();

    //get lines at the updated positions
    pt1 = eo1->childPtrs[0]->getPosition();
    pt2 = eo2->childPtrs[0]->getPosition();
    e1 = eo1->childPtrs[1]->getPosition() - pt1;
    e2 = eo2->childPtrs[1]->getPosition() - pt2;

    dist = pt2 - pt1;

    //find whether the lines intersect inside the marked segments
    float s = (glm::dot(dist, glm::cross(glm::normalize(e2), plane)))/(glm::dot(e1, glm::cross(glm::normalize(e2), plane)));
    float t = -(glm::dot(dist, glm::cross(glm::normalize(e1), plane)))/(glm::dot(e2, glm::cross(glm::normalize(e1), plane)));

    //if they meet outside the segments just return.
    //HACK ALERT: I don't handle the time left correctly in the collision detection loop, and so this is all wrong and I need
    //to go back through all of my collision handlers to fix it all and I'll never actually get around to it but I just need
    //to remember that this is a a whole thing.
    timeLeft = timeLeft * (1-f);
    if(s < 0 || s > 1 || t < 0 || t > 1) return timeLeft;

    glm::vec3 velColl1 = s*eo1->childPtrs[0]->getVelocity() + (1-s)*eo1->childPtrs[1]->getVelocity();
    glm::vec3 velColl2 = t*eo2->childPtrs[0]->getVelocity() + (1-t)*eo2->childPtrs[1]->getVelocity();
    glm::vec3 deltaV1 = velColl1 + velColl2;
    glm::vec3 pDeltaV1 = deltaV1/(s * s + (1-s) * (1-s));
    glm::vec3 pDeltaV2 = -deltaV1/(t * t + (1-t) * (1-t));
    eo1->childPtrs[0]->addVelocity(s * pDeltaV1);
    eo1->childPtrs[1]->addVelocity((1-s) * pDeltaV1);
    eo2->childPtrs[0]->addVelocity(t * pDeltaV2);
    eo2->childPtrs[1]->addVelocity((1-t) * pDeltaV2);

    eo1->getNextChildStates(timeLeft);
    eo2->getNextChildStates(timeLeft);
    eo1->getNextState(timeLeft);
    eo2->getNextState(timeLeft);

    return timeLeft;

}

float PhysicsManager::spherePlaneDet(SphereObject * sph, PlaneObject * pla)
{
    glm::vec3 spherePos = sph->getPosition();
    glm::vec3 nextSpherePos = sph->getNewPosition();
    float radius = sph->geoDescription.radius;
    glm::vec3 planePos = pla->getPosition();
    glm::vec3 planeNorm = pla->geoDescription.normal;

    glm::vec3 direction = nextSpherePos - spherePos;

    if(glm::dot(direction, planeNorm) > 0) planePos = planePos - (radius * planeNorm);
    else if(glm::dot(direction, planeNorm) < 0) planePos = planePos + (radius * planeNorm);

    float origDist = (planeNorm.x * spherePos.x) + (planeNorm.y * spherePos.y) + (planeNorm.z * spherePos.z) + -(glm::dot(planePos, planeNorm));
    float newDist = (planeNorm.x * nextSpherePos.x) + (planeNorm.y * nextSpherePos.y) + (planeNorm.z * nextSpherePos.z) + -(glm::dot(planePos, planeNorm));
    return origDist / (origDist - newDist);
}

int PhysicsManager::pointLSeg2D(glm::vec2 pt, glm::vec2 l0, glm::vec2 l1)
{
    if(l0.x < pt.x && l1.x < pt.x) return 0;
    if(l0.y < pt.y && l1.y < pt.y) return 0;
    if(l0.y > pt.y && l1.y > pt.y) return 0;
    if(l0.x > pt.x && l1.x > pt.x && l0.y > pt.y && l1.y > pt.y) return 0;
    if(l0.x > pt.x && l1.x > pt.x && l0.y < pt.y && l1.y < pt.y) return 0;
    if(l0.x > pt.x && l1.x > pt.x && l0.y >= pt.y && l1.y <= pt.y) return 1;
    if(l0.x > pt.x && l1.x > pt.x && l0.y <= pt.y && l1.y >= pt.y) return 1;
    glm::vec2 lineVec = glm::normalize(l1 - l0);
    float f;
    if(lineVec.y == 0) f = 0;
    else f = (pt.y - l0.y)/lineVec.y;
    float xf = l0.x + lineVec.x * f;
    if(pt.x <= xf) return 1;
    else return 0;

}



































