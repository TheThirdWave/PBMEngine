#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include <algorithm>
#include <math.h>

#include "../glm-0.9.8.5/glm/glm.hpp"
#include "../glm-0.9.8.5/glm/gtc/matrix_transform.hpp"
#include "../glm-0.9.8.5/glm/gtc/type_ptr.hpp"
#include "../glm-0.9.8.5/glm/gtx/transform.hpp"

#include "physicsobject.h"
#include "sphereobject.h"
#include "planeobject.h"
#include "structpile.h"


class PhysicsManager
{
private:
    PhysicsObject* objList[MAX_PHYS_OBJECTS];
    float       scalarGlobalForces[MAX_FORCES];
    glm::vec3   directonalGlobalForces[MAX_FORCES];
    int objLen, scaGFLen, dirGFLen;
    float elasticity;
    float fcoefficient;
public:
    PhysicsManager();
    void addPhysObj(PhysicsObject*);
    void addScalarForce(float);
    void addDirectionalForce(glm::vec3);
    void runTimeStep(float);
    float detectCollision(PhysicsObject*, PhysicsObject*, float);
    float determineCollision(PhysicsObject*, PhysicsObject*, float);
    float spherePlane(SphereObject*, PlaneObject*, float);
    float spherePlaneDet(SphereObject*, PlaneObject*);
};

#endif // PHYSICSMANAGER_H
