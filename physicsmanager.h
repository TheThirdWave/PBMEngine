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
#include "polygonobject.h"
#include "particleobject.h"
#include "particlegenerator.h"
#include "edgeobject.h"
#include "structpile.h"


class PhysicsManager
{
private:
    PhysicsObject* objList[MAX_PHYS_OBJECTS];
    ParticleObject* partList;
    ParticleGenerator** generators;
    float       scalarGlobalForces[MAX_FORCES];
    glm::vec3   directonalGlobalForces[MAX_FORCES];
    geometry    attractorGlobalForces[MAX_FORCES];

    int objLen, scaGFLen, dirGFLen, attGFLen, genLen, partLen;
    float elasticity;
    float fcoefficient;
public:
    PhysicsManager();
    void addPhysObj(PhysicsObject*);
    void addScalarForce(float);
    void addDirectionalForce(glm::vec3);
    void addAttractorForce(geometry);
    void addParticleGen(ParticleGenerator*);
    void addParticleList(ParticleObject*, int);
    void runTimeStep(float);
    void runRK4TimeStep(float);
    float detectCollision(PhysicsObject*, PhysicsObject*, float);
    float determineCollision(PhysicsObject*, PhysicsObject*, float);
    float spherePlane(SphereObject*, PlaneObject*, float);
    float spherePoly(SphereObject*, PolygonObject*, float);
    float partPoly(ParticleObject*, PolygonObject*, float);
    float edgeEdge(EdgeObject*, EdgeObject*, float);
    float spherePlaneDet(SphereObject*, PlaneObject*);
    int pointLSeg2D(glm::vec2, glm::vec2, glm::vec2);
};

#endif // PHYSICSMANAGER_H
