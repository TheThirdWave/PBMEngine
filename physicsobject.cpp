#include "physicsobject.h"

PhysicsObject::PhysicsObject()
{
    curState.rotation = glm::vec3(0.0f);
    curState.position = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    curState.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    curState.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    mass = 1;
    ttl = -1;
    rendrPtr = NULL;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, glm::vec3 pos, glm::vec3 vel, glm::vec3 accel)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    curState.position = pos;
    scale = glm::vec3(1.0f);
    curState.velocity = vel;
    curState.acceleration = accel;
    mass = 1;
    ttl = -1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, glm::vec3 pos, glm::vec3 vel)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    curState.position = pos;
    scale = glm::vec3(1.0f);
    curState.velocity = vel;
    curState.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    ttl = -1;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, glm::vec3 pos)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    curState.position = pos;
    scale = glm::vec3(1.0f);
    curState.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    curState.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    ttl = -1;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(RenderObject* ptr, float x, float y, float z)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = ptr;
    curState.position = glm::vec3(x, y, z);
    curState.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    curState.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    ttl = -1;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(glm::vec3 pos, glm::vec3 vel, glm::vec3 accel)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    curState.position = pos;
    curState.velocity = vel;
    scale = glm::vec3(1.0f);
    curState.acceleration = accel;
    mass = 1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    ttl = -1;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(glm::vec3 pos, glm::vec3 vel)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    curState.position = pos;
    scale = glm::vec3(1.0f);
    curState.velocity = vel;
    curState.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    ttl = -1;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(glm::vec3 pos)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    curState.position = pos;
    scale = glm::vec3(1.0f);
    curState.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    curState.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    ttl = -1;
    numChildren = 0;
    numParents = 0;
}

PhysicsObject::PhysicsObject(float x, float y, float z)
{
    curState.rotation = glm::vec3(0.0f);
    rendrPtr = NULL;
    curState.position = glm::vec3(x, y, z);
    curState.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f);
    curState.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    mass = 1;
    newState.position = curState.position;
    newState.velocity = curState.velocity;
    ttl = -1;
    numChildren = 0;
    numParents = 0;
}

void PhysicsObject::updateRenderObject()
{
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), curState.position);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * sca);
}

void PhysicsObject::setRenderObject(RenderObject* ptr)
{
    rendrPtr = ptr;
}

void PhysicsObject::setNextFromCurrent()
{
    newState.velocity = curState.velocity;
    newState.position = curState.position;
    newState.rotation = curState.rotation;
}

void PhysicsObject::setRotation(glm::vec3 rot)
{
    curState.rotation = rot;
}

void PhysicsObject::setNewRotation(glm::vec3 rot)
{
    newState.rotation = rot;
}

void PhysicsObject::setPosition(glm::vec3 pos)
{
    curState.position = pos;
}

void PhysicsObject::setScale(glm::vec3 sca)
{
    scale = sca;
}

void PhysicsObject::setNewPosition(glm::vec3 pos)
{
    newState.position = pos;
}

void PhysicsObject::setVelocity(glm::vec3 vel)
{
    curState.velocity = vel;
}

void PhysicsObject::setNewVelocity(glm::vec3 vel)
{
    newState.velocity = vel;
}

void PhysicsObject::setAcceleration(glm::vec3 accel)
{
    curState.acceleration = accel / mass;
}

void PhysicsObject::setMass(float ma)
{
    mass = ma;
}

void PhysicsObject::setID(int i)
{
    id = i;
}

void PhysicsObject::setTTL(double t)
{
    ttl = t;
}

void PhysicsObject::setGeometry(geometry geo)
{
    geoDescription = geo;
}

void PhysicsObject::addChild(PhysicsObject* c)
{
    if(numChildren == 0) childPtrs = new PhysicsObject*[MAX_CHILDREN];
    childPtrs[numChildren++] = c;
}

void PhysicsObject::addParent(PhysicsObject* p)
{
    if(numParents == 0) parentPtrs = new PhysicsObject*[MAX_PARENTS];
    parentPtrs[numParents++] = p;
}

void PhysicsObject::addRotation(glm::vec3 nr)
{
    curState.rotation = curState.rotation + nr;
}

void PhysicsObject::addNewRotation(glm::vec3 nr)
{
    newState.rotation = newState.rotation + nr;
}

void PhysicsObject::addPosition(glm::vec3 np)
{
    curState.position = curState.position + np;
}

void PhysicsObject::addVelocity(glm::vec3 nv)
{
    curState.velocity = curState.velocity + nv;
}

void PhysicsObject::addScale(glm::vec3 sc)
{
    scale = scale + sc;
}

void PhysicsObject::addAcceleration(glm::vec3 na)
{
    derivStates[0].acceleration = derivStates[0].acceleration + (na / mass);
}

void PhysicsObject::getNextState(float ts)
{
    newState.velocity = curState.velocity + (derivStates[0].acceleration * ts);
    newState.position = curState.position + (newState.velocity * ts);
}

void PhysicsObject::getNextState(int idx)
{
    newState.velocity = newState.velocity + derivStates[idx].velocity;
    newState.position = newState.position + derivStates[idx].position;
}


void PhysicsObject::getNextRKState(float ts, int idx)
{
    derivStates[idx].velocity = (derivStates[idx].acceleration * ts);
    derivStates[idx].position =  (newState.velocity + derivStates[idx].velocity) * ts;
}

int PhysicsObject::getChildIdx(PhysicsObject* ptr)
{
    if(numChildren <= 0) return -1;
    for(int i = 0; i < numChildren; i++)
    {
        if(childPtrs[i] == ptr) return i;
    }
    return -1;
}

int PhysicsObject::getId()
{
    return id;
}

int PhysicsObject::getCollections(int size, int* buf)
{
   int count = 0;
   for(int i = 0; i < numParents; i++)
   {
       if(parentPtrs[i]->getId() == COLLECTION) buf[count++] = i;
       if(count >= size) return count;
   }
   return count;
}

int PhysicsObject::getVertices(int buf[])
{
    int count = 0;
    for(int i = 0; i < numChildren; i++)
    {
        if(childPtrs[i]->getId() == 3) buf[count++] = i;
    }
    return count;
}

int PhysicsObject::getEdges(int buf[])
{
    int count = 0;
    for(int i = 0; i < numChildren; i++)
    {
        if(childPtrs[i]->getId() == 4) buf[count++] = i;
    }
    return count;
}


void PhysicsObject::updateState()
{
    curState = newState;
}

float PhysicsObject::getVelMag()
{
    return glm::length(curState.velocity);
}

float PhysicsObject::getAccelMag()
{
    return glm::length(curState.acceleration);
}

float PhysicsObject::getMass()
{
    return mass;
}

glm::vec3 PhysicsObject::getPosition(){
    return curState.position;
}

glm::vec3 PhysicsObject::getVelocity(){
    return curState.velocity;
}

glm::vec3 PhysicsObject::getNewPosition()
{
    return newState.position;
}

glm::vec3 PhysicsObject::getNewVelocity()
{
    return newState.velocity;
}

RenderObject* PhysicsObject::getRenderObj()
{
    return rendrPtr;
}

bool PhysicsObject::checkCollections(PhysicsObject* ptr)
{
    int col[MAX_PARENTS];
    int count = getCollections(MAX_PARENTS, col);
    if(count == 0) return false;
    else
    {
        for(int i = 0; i < count; i++)
        {
            if(parentPtrs[i]->getChildIdx(ptr) > -1) return true;
        }
    }
    return false;
}

void PhysicsObject::initRenderObj()
{
}
