#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "../glm-0.9.8.5/glm/glm.hpp"
#include "../glm-0.9.8.5/glm/gtc/matrix_transform.hpp"
#include "../glm-0.9.8.5/glm/gtc/type_ptr.hpp"
#include "../glm-0.9.8.5/glm/gtx/transform.hpp"
#include "../glm-0.9.8.5/glm/gtx/rotate_vector.hpp"


#include "renderobject.h"
#include "structpile.h"

class RenderObject;

class PhysicsObject
{
   friend class PhysicsManager;

protected:
   PhysicsObject** childPtrs;
   PhysicsObject** parentPtrs;
   RenderObject* rendrPtr;
   void* manager;

   glm::vec3 scale;

   int index;
   int numChildren, numParents;

public:
    PhysicsObject(void);
    PhysicsObject(void*);
    void initRenderObj();
    virtual void updateRenderObject();
    virtual void setRenderObject(RenderObject*);
    virtual void setNextFromCurrent();
    void setRotation(glm::vec3);
    void setNewRotation(glm::vec3);
    void setPosition(glm::vec3);
    void setNewPosition(glm::vec3);
    void setVelocity(glm::vec3);
    void setNewVelocity(glm::vec3);
    void setAcceleration(glm::vec3);
    void setScale(glm::vec3);
    void setMass(float);
    void setID(int);
    void setTTL(double);
    void setGeometry(geometry);
    void setAlive(bool);
    void setActive(bool);
    void setSolid(bool);
    void setManager(void* mngr);
    void addToManager(void* mngr);
    void addChild(PhysicsObject*);
    void addParent(PhysicsObject*);
    void addRotation(glm::vec3);
    void addNewRotation(glm::vec3);
    void addPosition(glm::vec3);
    void addVelocity(glm::vec3);
    void addAcceleration(glm::vec3);
    void addScale(glm::vec3);
    virtual void getNextState(float);
    virtual void updateState();
    void getNextState(int);
    virtual void getNextRKState(float, int);
    float getVelMag();
    float getAccelMag();
    float getMass();
    int getChildIdx(PhysicsObject*);
    int getCollections(int, int[]);
    int getId();
    int getIndex();
    int getVertices(int[]);
    int getEdges(int[]);
    glm::vec3 getPosition();
    glm::vec3 getVelocity();
    glm::vec3 getNewPosition();
    glm::vec3 getNewVelocity();
    glm::vec3 getRotation();
    RenderObject* getRenderObj();
    geometry getGeometry();
    bool checkCollections(PhysicsObject*);

};

#endif // PHYSICSOBJECT_H
