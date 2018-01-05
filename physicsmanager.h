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
#include "objcollection.h"
#include "structpile.h"


class PhysicsManager
{
    friend class PhysicsObject;
    friend class PolygonObject;
    friend class EdgeObject;
    friend class SphereObject;
    friend class PlaneObject;
    friend class ParticleObject;
    friend class ObjCollection;

private:
    ParticleObject* partList;
    ParticleGenerator** generators;
    float       scalarGlobalForces[MAX_FORCES];
    glm::vec3   directonalGlobalForces[MAX_FORCES];
    geometry    attractorGlobalForces[MAX_FORCES];
    state       curState[MAX_PHYS_OBJECTS];
    state       nextState[MAX_PHYS_OBJECTS];
    state       derivStates[NUM_DERIV_STATES][MAX_PHYS_OBJECTS];
    attributes  attribs[MAX_PHYS_OBJECTS];
    PhysicsObject* objList[MAX_PHYS_OBJECTS];

    int objLen, scaGFLen, dirGFLen, attGFLen, genLen, partLen, sPrecision;
    float elasticity;
    float fcoefficient;
public:
    PhysicsManager();
    int addPhysObj(PhysicsObject*);
    void addScalarForce(float);
    void addDirectionalForce(glm::vec3);
    void addAttractorForce(geometry);
    void addParticleGen(ParticleGenerator*);
    void addParticleList(ParticleObject*, int);
    void runTimeStep(float);
    void runRK4TimeStep(float);
    void setNextFromCurState();
    void getNextRK4(float);
    void getDerivFromNextState(int);
    void addIntegralToNS(float, int);
    void combineRK4DerivStates(float);
    float detectCollision(int idx1, int idx2, float);
    float determineCollision(PhysicsObject*, PhysicsObject*, float);
    float spherePlane(int, int, float);
    float spherePoly(int, int, float);
    float partPoly(int, int, float);
    float edgeEdge(int, int, float);
    int pointLSeg2D(glm::vec2, glm::vec2, glm::vec2);
};

#endif // PHYSICSMANAGER_H
