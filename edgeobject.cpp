#include "edgeobject.h"
#include "physicsmanager.h"

EdgeObject::EdgeObject()
{

}

EdgeObject::EdgeObject(void* mngr)
{
    manager = mngr;
    scale = glm::vec3(1.0f);
    rendrPtr = NULL;
    numChildren = 0;
    numParents = 0;

    PhysicsManager* boop = (PhysicsManager*) manager;
    index = boop->addPhysObj(this);
    setID(EDGE);
}

void EdgeObject::addChild(PhysicsObject* c)
{
    if(numChildren < MAX_EDGE_CHILDREN)
    {
        if(numChildren == 0) childPtrs = new PhysicsObject*[MAX_POLYGON_CHILDREN];
        childPtrs[numChildren++] = c;
    }
    else
    {
        fprintf(stderr, "Edge cannot take more children.\n");
    }
}

void EdgeObject::setSpring(float L, float D, float K)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].springL = L;
    boop->attribs[index].springD = D;
    boop->attribs[index].springK = K;
}

void EdgeObject::getNextState(float ts)
{

}

void EdgeObject::getNextRKState(float ts, int idx)
{
    getNextState(ts);
}

void EdgeObject::getNextChildStates(float ts)
{
//    childPtrs[0]->getNextState(ts);
//    childPtrs[1]->getNextState(ts);
}

void EdgeObject::updateChildren()
{
//    childPtrs[0]->updateState();
//    childPtrs[1]->updateState();
}

void EdgeObject::updateState()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index].position = (childPtrs[0]->getPosition() + childPtrs[1]->getPosition());
    boop->nextState[index].position = (glm::length(boop->nextState[index].position) / 2) * glm::normalize(boop->nextState[index].position);
    boop->nextState[index].velocity = boop->nextState[index].position - boop->curState[index].position;
}

void EdgeObject::setRenderObject(RenderObject * e)
{

}

void EdgeObject::updateRenderObject()
{

}
