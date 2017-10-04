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
   RenderObject* rendrPtr;
   glm::vec3 rotation;
   glm::vec3 position;
   glm::vec3 newPosition;
   glm::vec3 velocity;
   glm::vec3 newVelocity;
   glm::vec3 acceleration;
   glm::vec3 scale;
   geometry geoDescription;

   double ttl;
   float mass;
   int id;

public:
    PhysicsObject(void);
    PhysicsObject(RenderObject*, glm::vec3, glm::vec3, glm::vec3);
    PhysicsObject(RenderObject*, glm::vec3, glm::vec3);
    PhysicsObject(RenderObject*, glm::vec3);
    PhysicsObject(RenderObject*, float, float, float);
    PhysicsObject(glm::vec3, glm::vec3, glm::vec3);
    PhysicsObject(glm::vec3, glm::vec3);
    PhysicsObject(glm::vec3);
    PhysicsObject(float, float, float);
    void initRenderObj();
    virtual void updateRenderObject();
    virtual void setRenderObject(RenderObject*);
    void setRotation(glm::vec3);
    void setPosition(glm::vec3);
    void setNewPosition(glm::vec3);
    void setVelocity(glm::vec3);
    void setNewVelocity(glm::vec3);
    void setAcceleration(glm::vec3);
    void setScale(glm::vec3);
    void setMass(float);
    void setTTL(double);
    void setGeometry(geometry);
    void addRotation(glm::vec3);
    void addPosition(glm::vec3);
    void addVelocity(glm::vec3);
    void addAcceleration(glm::vec3);
    void addScale(glm::vec3);
    void getNextState(float);
    virtual void updateState();
    float getVelMag();
    float getAccelMag();
    glm::vec3 getPos();
    RenderObject* getRenderObj();

};

#endif // PHYSICSOBJECT_H
