#include "physicsmanager.h"

PhysicsManager::PhysicsManager()
{
    scaGFLen = 0;
    dirGFLen = 0;
    objLen = 0;
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

void PhysicsManager::runTimeStep(float ts)
{
    //set accelerations
    for(int i = 0; i < objLen; i++)
    {
        glm::vec3 hold = glm::vec3(0.0f);
        for(int j = 0; j < dirGFLen; j++)
        {
            hold = hold + directonalGlobalForces[j];
        }
        for(int j = 0; j < scaGFLen; j++)
        {
            glm::vec3 blah = objList[i]->velocity * scalarGlobalForces[j];
            hold = hold + blah;
        }
        objList[i]->setAcceleration(hold);
    }

    float timeLeft = ts;
    //calculate next state
    for(int i = 0; i < objLen; i++)
    {
        objList[i]->getNextState(ts);
    }

    //check for collisions
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

    //update state
    for(int i = 0; i< objLen; i++)
    {
        objList[i]->updateState();
        objList[i]->updateRenderObject();
    }
}

float PhysicsManager::detectCollision(PhysicsObject* one, PhysicsObject* two, float timeLeft)
{
    if(one->id == SPHERE && two->id == PLANE) return spherePlane((SphereObject*)one, (PlaneObject*)two, timeLeft);
    if(one->id == PLANE && two->id == SPHERE) return spherePlane((SphereObject*)two, (PlaneObject*)one, timeLeft);
    if(one->id == SPHERE && two->id == POLYGON) return spherePoly((SphereObject*)one, (PolygonObject*)two, timeLeft);
    if(one->id == POLYGON && two->id == SPHERE) return spherePoly((SphereObject*)two, (PolygonObject*)one, timeLeft);
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
    glm::vec3 spherePos = sph->position;
    glm::vec3 nextSpherePos = sph->newPosition;
    float radius = sph->geoDescription.radius;
    glm::vec3 planePos = pla->position;
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

    glm::vec3 cross = glm::dot(sph->velocity, planeNorm) * planeNorm;
    glm::vec3 up = -elasticity * cross;
    glm::vec3 accross = sph->velocity - cross;

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
    glm::vec3 spherePos = sph->position;
    glm::vec3 nextSpherePos = sph->newPosition;
    float radius = sph->geoDescription.radius;
    glm::vec3 planePos = pla->position;
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
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), pla->position);
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
    float m = std::max(planeNorm.x, planeNorm.y);
    m = std::max(m, planeNorm.z);
    int numIntercepts = 0;
    for(int i = 0; i < numTris; i++)
    {
        if(m == planeNorm.x)
        {
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.y, sph->position.z), glm::vec2(tris[i].a.y, tris[i].a.z), glm::vec2(tris[i].b.y, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.y, sph->position.z), glm::vec2(tris[i].c.y, tris[i].c.z), glm::vec2(tris[i].b.y, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.y, sph->position.z), glm::vec2(tris[i].a.y, tris[i].a.z), glm::vec2(tris[i].c.y, tris[i].c.z));
        }
        else if(m == planeNorm.y)
        {
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.x, sph->position.z), glm::vec2(tris[i].a.x, tris[i].a.z), glm::vec2(tris[i].b.x, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.x, sph->position.z), glm::vec2(tris[i].c.x, tris[i].c.z), glm::vec2(tris[i].b.x, tris[i].b.z));
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.x, sph->position.z), glm::vec2(tris[i].a.x, tris[i].a.z), glm::vec2(tris[i].c.x, tris[i].c.z));
        }
        else
        {
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.x, sph->position.y), glm::vec2(tris[i].a.x, tris[i].a.y), glm::vec2(tris[i].b.x, tris[i].b.y));
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.x, sph->position.y), glm::vec2(tris[i].c.x, tris[i].c.y), glm::vec2(tris[i].b.x, tris[i].b.y));
            numIntercepts += pointLSeg2D(glm::vec2(sph->position.x, sph->position.y), glm::vec2(tris[i].a.x, tris[i].a.y), glm::vec2(tris[i].c.x, tris[i].c.y));
        }
    }
    timeLeft = timeLeft * (1-f);
    if(numIntercepts % 2 == 0) return timeLeft;

    glm::vec3 cross = glm::dot(sph->velocity, planeNorm) * planeNorm;
    glm::vec3 up = -elasticity * cross;
    glm::vec3 accross = sph->velocity - cross;

    if(glm::length(accross) != 0)
    {
        accross = accross - std::min(fcoefficient * sph->mass * glm::length(up), glm::length(accross)) * glm::normalize(accross);
    }

    glm::vec3 fnVec = up + accross;

    sph->setVelocity(fnVec);
    sph->getNextState(timeLeft);


    return timeLeft;

}

float PhysicsManager::spherePlaneDet(SphereObject * sph, PlaneObject * pla)
{
    glm::vec3 spherePos = sph->position;
    glm::vec3 nextSpherePos = sph->newPosition;
    float radius = sph->geoDescription.radius;
    glm::vec3 planePos = pla->position;
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
    glm::vec2 lineVec = glm::normalize(l1 - l0);
    float f = (pt.y - l0.y)/lineVec.y;
    float xf = l0.x + lineVec.x * f;
    if(pt.x <= xf) return 1;
    else return 0;

}



































