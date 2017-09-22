#include "physicsmanager.h"

PhysicsManager::PhysicsManager()
{
    scaGFLen = 0;
    dirGFLen = 0;
    objLen = 0;
    elasticity = 0.1f;
    fcoefficient = 0.9f;
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

    sph->getNextState(timeLeft * f);
    sph->updateState();

    glm::vec3 cross = glm::dot(sph->velocity, planeNorm) * planeNorm;
    glm::vec3 up = -elasticity * cross;
    glm::vec3 accross = sph->velocity - cross;

    if(glm::length(accross) != 0)
    {
        accross = accross - std::min(fcoefficient * sph->mass * glm::length(cross), glm::length(accross)) * glm::normalize(accross);
    }
    glm::vec3 fnVec = up + accross;

    sph->setVelocity(fnVec);
    timeLeft = timeLeft * (1-f);
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











