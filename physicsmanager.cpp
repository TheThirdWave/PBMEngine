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
    getAccels();

    getNState(ts);

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
                        timeLeft = detectCollision(objList[i], objList[j], timeLeft);
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

void PhysicsManager::runRK4TimeStep(float ts)
{
    float timeLeft = ts;
    while(ts > 0)
    {
        getAccelsRK4(ts);
        getNStateRK4(ts);


        //check for collisions for objects
        for(int i = 0; i < objLen-1; i++)
        {
            if(objList[i]->alive && objList[i]->solid)
            {
                for(int j = i + 1; j < objLen; j++)
                {
                    if(objList[j]->alive && objList[j]->solid)
                    {
                        timeLeft = detectCollision(objList[i], objList[j], timeLeft);
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

        if(ts == timeLeft) ts = 0;
        else ts = timeLeft;

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
}

void PhysicsManager::getAccels()
{
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
        else if(objList[i]->id == EDGE)
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
}

void PhysicsManager::getAccelsRK4(float ts)
{
    for(int f = 0; f < NUM_DERIV_STATES; f++)
    {

        //forces
        for(int i = 0; i < objLen; i++)
        {
            if(f == 0) objList[i]->setNextFromCurrent();
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
                    glm::vec3 blah = objList[i]->newState.velocity * scalarGlobalForces[j];
                    hold = hold + blah;
                }
                for(int j = 0; j < attGFLen; j++)
                {
                    glm::vec3 point = glm::normalize(attractorGlobalForces[j].normal - objList[i]->newState.position);
                    hold = hold + (point * attractorGlobalForces[j].radius);
                }
                objList[i]->derivStates[f].acceleration = hold / objList[i]->getMass();
            }
            //spring forces.
            else if(objList[i]->id == EDGE)
            {
                glm::vec3 pt1 = objList[i]->childPtrs[0]->newState.position;
                glm::vec3 pt2 = objList[i]->childPtrs[1]->newState.position;
                glm::vec3 v1 = objList[i]->childPtrs[0]->newState.velocity;
                glm::vec3 v2 = objList[i]->childPtrs[1]->newState.velocity;
                glm::vec3 pt12 = pt2 - pt1;
                glm::vec3 npt12 = glm::normalize(pt12);
                glm::vec3 sf = objList[i]->springK * (glm::length(pt12) - objList[i]->springL) * npt12;
                glm::vec3 df = objList[i]->springD * (glm::dot((v2 - v1), npt12)) * npt12;
                objList[i]->childPtrs[0]->derivStates[f].acceleration += (sf + df) / objList[i]->childPtrs[0]->getMass();
                objList[i]->childPtrs[1]->derivStates[f].acceleration += (-sf + -df) / objList[i]->childPtrs[1]->getMass();
            }
        }

        for(int fi = 0; fi < NUM_DERIV_STATES; fi++)
        {
            //calculate next state
            for(int i = 0; i < objLen; i++)
            {
                if(objList[i]->alive && objList[i]->active)
                {
                    if(fi < 3) objList[i]->getNextRKState(ts/2, fi);
                    else objList[i]->getNextRKState(ts, fi);
                    objList[i]->setNextFromCurrent();
                    objList[i]->getNextState(fi);
                }
            }

        }
    }
}

void PhysicsManager::getNState(float ts)
{
    for(int i = 0; i < objLen; i++)
    {
        objList[i]->getNextState(ts);
    }
}

void PhysicsManager::getNStateRK4(float ts)
{
    for(int i = 0; i < objLen; i++)
    {
        glm::vec3 position = objList[i]->derivStates[1].position;
        glm::vec3 velocity = objList[i]->derivStates[1].velocity;
        glm::vec3 acceleration = objList[i]->derivStates[1].acceleration;
        objList[i]->derivStates[0].position += position * 2.0f;
        objList[i]->derivStates[0].velocity += velocity * 2.0f;
        objList[i]->derivStates[0].acceleration += acceleration * 2.0f;
        objList[i]->derivStates[0].position += objList[i]->derivStates[2].position * 2.0f;
        objList[i]->derivStates[0].velocity += objList[i]->derivStates[2].velocity * 2.0f;
        objList[i]->derivStates[0].acceleration += objList[i]->derivStates[2].acceleration * 2.0f;
        objList[i]->derivStates[0].position += objList[i]->derivStates[3].position;
        objList[i]->derivStates[0].velocity += objList[i]->derivStates[3].velocity;
        objList[i]->derivStates[0].acceleration += objList[i]->derivStates[3].acceleration;

        objList[i]->derivStates[0].position = objList[i]->derivStates[0].position * (1/6.0f);
        objList[i]->derivStates[0].velocity = objList[i]->derivStates[0].velocity * (1/6.0f);
        objList[i]->derivStates[0].acceleration = objList[i]->derivStates[0].acceleration * (1/6.0f);
        objList[i]->setNextFromCurrent();
        objList[i]->getNextState(0);
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
    if(one->id == EDGE && two->id == EDGE)
    {
        return edgeEdge((EdgeObject*)one, (EdgeObject*)two, timeLeft);
    }
    return timeLeft;
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
    if(!(newDist < 0 && origDist > 0 || newDist > 0 && origDist < 0 || newDist == 0 && origDist == 0)) return timeLeft;


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
    if(pla->getChildIdx((PhysicsObject*)par) >= 0) return timeLeft;
    if(pla->checkCollections((PhysicsObject*)par)) return timeLeft;
    glm::vec3 spherePos = par->getPosition();
    glm::vec3 nextSpherePos = par->getNewPosition();
    glm::vec3 planePos = pla->getPosition();
    glm::vec3 planeNorm = pla->geoDescription.normal;
    glm::vec3 planeUp = pla->geoDescription.upVec;

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
    getNStateRK4(middle);
    for(int i = 0; i < sPrecision; i++)
    {
        spherePos = par->getPosition();
        nextSpherePos = par->getNewPosition();
        planePos = pla->getPosition();
        planeNorm = pla->geoDescription.normal;
        planeUp = pla->geoDescription.upVec;

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
        getNStateRK4(middle);
    }

    timeLeft = timeLeft * (1-f);
    if(collision == false) return timeLeft;

    glm::vec3 point = nextSpherePos - spherePos;
    float len = std::abs(glm::dot((point), planeNorm));
    len = glm::dot((spherePos - planePos), planeNorm) / len;
    getNStateRK4(timeLeft * len);
    nextSpherePos = spherePos + (point * len);
    nextSpherePos = pla->getNewPosition();


    //get the points from the children of the polygon to use as collision edges.
    int childs[MAX_POLYGON_CHILDREN];
    int count = pla->getVertices(childs);

    //Get the barycentric coordinates of the point of collision on the plane. (We're assuming that the "polygon" is a triangle here)
    glm::vec3 e01 = pla->childPtrs[childs[1]]->getPosition() - pla->childPtrs[childs[0]]->getPosition();
    glm::vec3 e12 = pla->childPtrs[childs[2]]->getPosition() - pla->childPtrs[childs[1]]->getPosition();
    glm::vec3 e20 = pla->childPtrs[childs[0]]->getPosition() - pla->childPtrs[childs[2]]->getPosition();
    glm::vec3 p1x = nextSpherePos - pla->childPtrs[childs[1]]->getPosition();
    glm::vec3 p2x = nextSpherePos - pla->childPtrs[childs[2]]->getPosition();
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
    if(pla->active)
    {
        float addedWeights = 0;
        float* weights = new float[pla->numChildren];
        float mass = 0;
        float cWeights = 0;
        glm::vec3 velCollPoly = glm::vec3(0.0f);
        for(int i = 0; i< pla->numChildren; i++) addedWeights += glm::length(par->getPosition() - pla->childPtrs[i]->getPosition());
        for(int i = 0; i < pla->numChildren; i++)
        {
            weights[i] = 1 - (glm::length(par->getPosition() - pla->childPtrs[i]->getPosition()) / addedWeights);
            cWeights += weights[i] * weights[i];
            mass += weights[i] * pla->childPtrs[i]->getMass();
            velCollPoly += weights[i] * pla->childPtrs[i]->getVelocity();
        }

        float totalMass = mass / cWeights;

        //to get the center of momentum we add the moments of inertia of both objects (the face and the vector) and divide by the masses.

        glm::vec3 com = (par->getMass() * par->getVelocity() + totalMass * velCollPoly) / (par->getMass() + totalMass);

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
        for(int i = 0; i < pla->numChildren; i++)
        {
            pla->childPtrs[i]->setVelocity((deltaV1 + com) * weights[i]);
            pla->childPtrs[i]->getNextState(timeLeft);
        }
        pla->getNextState(timeLeft);
    }
    else
    {
        glm::vec3 deltaV2 = (par->getVelocity());
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

float PhysicsManager::edgeEdge(EdgeObject * eo1, EdgeObject * eo2, float timeLeft)
{
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
    getNStateRK4(timePassed);

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



































