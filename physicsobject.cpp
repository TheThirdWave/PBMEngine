#include "physicsobject.h"
#include "physicsmanager.h"

PhysicsObject::PhysicsObject()
{
    scale = glm::vec3(1.0f);
    rendrPtr = NULL;
    numChildren = 0;
    numParents = 0;
    manager = NULL;
}

PhysicsObject::PhysicsObject(void* mngr)
{
    manager = mngr;
    scale = glm::vec3(1.0f);
    rendrPtr = NULL;
    numChildren = 0;
    numParents = 0;
}

void PhysicsObject::updateRenderObject()
{
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), getPosition());
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * sca);
}

void PhysicsObject::setRenderObject(RenderObject* ptr)
{
    rendrPtr = ptr;
}

void PhysicsObject::setNextFromCurrent()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index] = boop->curState[index];
}

void PhysicsObject::setRotation(glm::vec3 rot)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->curState[index].rotation = rot;
}

void PhysicsObject::setNewRotation(glm::vec3 rot)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index].rotation = rot;
}

void PhysicsObject::setPosition(glm::vec3 pos)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->curState[index].position = pos;
}

void PhysicsObject::setScale(glm::vec3 sca)
{
    scale = sca;
}

void PhysicsObject::setNewPosition(glm::vec3 pos)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index].position = pos;
}

void PhysicsObject::setVelocity(glm::vec3 vel)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->curState[index].velocity = vel;
}

void PhysicsObject::setNewVelocity(glm::vec3 vel)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index].velocity = vel;
}

void PhysicsObject::setAcceleration(glm::vec3 accel)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->curState[index].acceleration = accel;
}

void PhysicsObject::setMass(float ma)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].mass = ma;
}

void PhysicsObject::setID(int i)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].id = i;
}

void PhysicsObject::setTTL(double t)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].ttl = t;
}

void PhysicsObject::setGeometry(geometry geo)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].geo = geo;
}

void PhysicsObject::setActive(bool in)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].active = in;
}

void PhysicsObject::setAlive(bool in)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].alive = in;
}

void PhysicsObject::setSolid(bool in)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].solid = in;
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
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->curState[index].rotation += nr;
}

void PhysicsObject::addNewRotation(glm::vec3 nr)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index].rotation += nr;
}

void PhysicsObject::addPosition(glm::vec3 np)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->curState[index].position += np;
}

void PhysicsObject::addVelocity(glm::vec3 nv)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->curState[index].velocity += nv;
}

void PhysicsObject::addScale(glm::vec3 sc)
{
    scale = scale + sc;
}

void PhysicsObject::addAcceleration(glm::vec3 na)
{
}

void PhysicsObject::getNextState(float ts)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index].velocity = boop->curState[index].velocity + boop->nextState[index].acceleration * ts;
    boop->nextState[index].position = boop->curState[index].position + boop->nextState[index].velocity * ts;
}

void PhysicsObject::getNextState(int idx)
{

}


void PhysicsObject::getNextRKState(float ts, int idx)
{

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
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->attribs[index].id;
}

int PhysicsObject::getIndex()
{
    return index;
}

int PhysicsObject::getCollections(int size, int* buf)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
   int count = 0;
   for(int i = 0; i < numParents; i++)
   {
       if(boop->attribs[parentPtrs[i]->getIndex()].id == COLLECTION) buf[count++] = i;
       if(count >= size) return count;
   }
   return count;
}

int PhysicsObject::getVertices(int buf[])
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    int count = 0;
    for(int i = 0; i < numChildren; i++)
    {
        if(boop->attribs[childPtrs[i]->getIndex()].id == PARTICLE) buf[count++] = i;
    }
    return count;
}

int PhysicsObject::getEdges(int buf[])
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    int count = 0;
    for(int i = 0; i < numChildren; i++)
    {
        if(boop->attribs[childPtrs[i]->getIndex()].id == EDGE) buf[count++] = i;
    }
    return count;
}


void PhysicsObject::updateState()
{
}

float PhysicsObject::getVelMag()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    return glm::length(boop->curState[index].velocity);
}

float PhysicsObject::getAccelMag()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    return glm::length(boop->curState[index].acceleration);
}

float PhysicsObject::getMass()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->attribs[index].mass;
}

glm::vec3 PhysicsObject::getPosition(){
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->curState[index].position;
}

glm::vec3 PhysicsObject::getVelocity(){
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->curState[index].velocity;
}

glm::vec3 PhysicsObject::getNewPosition()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->nextState[index].position;
}

glm::vec3 PhysicsObject::getNewVelocity()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->nextState[index].velocity;
}

glm::vec3 PhysicsObject::getRotation()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->nextState[index].rotation;
}

RenderObject* PhysicsObject::getRenderObj()
{
    return rendrPtr;
}

geometry PhysicsObject::getGeometry()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    return boop->attribs[index].geo;
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
            if(parentPtrs[col[i]]->getChildIdx(ptr) > -1) return true;
        }
    }
    return false;
}

void PhysicsObject::initRenderObj()
{
}
