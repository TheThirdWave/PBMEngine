#ifndef OBJECTCREATOR_H
#define OBJECTCREATOR_H

#include "physicsmanager.h"
#include "physicsobject.h"
#include "edgeobject.h"
#include "polygonobject.h"
#include "particleobject.h"
#include "objcollection.h"
#include "particlegenerator.h"

class ObjectCreator
{
private:
    PhysicsManager* manager;
    EdgeObject* edges;
    PolygonObject* polys;
    ParticleObject* verticies;
    ObjCollection* collections;
    RenderObject* vModel;
    RenderObject* eModel;
    RenderObject* pModel;
    float springL, springD, springK;
    int maxEdges;
    int numEdges;
    int maxPolys;
    int numPolys;
    int maxVerts;
    int numVerts;
    int maxCols;
    int numCols;

public:
    ObjectCreator();
    void setEdgeArray(EdgeObject[], int);
    void setPolyArray(PolygonObject[], int );
    void setVertArray(ParticleObject[], int);
    void setColArray(ObjCollection[], int);
    void setPhysManager(PhysicsManager*);
    void setDefaultVModel(RenderObject*);
    void setDefaultEModel(RenderObject*);
    void setDefaultPModel(RenderObject*);
    void setSpringAttribs(float, float, float);

    int createVertex(glm::vec3);
    int createEdge(glm::vec3, glm::vec3);
    int createEdge(int, int);
    int createPoly(glm::vec3, glm::vec3, glm::vec3);
    int createPoly(int, int, int);
    int createSTriangle(glm::vec3, glm::vec3, glm::vec3);
    int createSTriangle(int, int, int);
    int createTriangle(glm::vec3, glm::vec3, glm::vec3);
    int createTriangle(int, int, int);
    int createCube(glm::vec3, float);

    int getNumPolys();
    int getNumEdges();
    int getNumVerts();
    int getNumCollections();

    void clearAllObjects();

};

#endif // OBJECTCREATOR_H
