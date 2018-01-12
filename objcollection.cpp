#include "objcollection.h"
#include "physicsmanager.h"

ObjCollection::ObjCollection()
{

}

ObjCollection::ObjCollection(void* mngr)
{
    manager = mngr;
    scale = glm::vec3(1.0f);
    rendrPtr = NULL;
    numChildren = 0;
    numParents = 0;

    PhysicsManager* boop = (PhysicsManager*) manager;
    index = boop->addPhysObj(this);
    setID(POLYGON);
}

void ObjCollection::initRenderObj()
{
}

void ObjCollection::setNextFromCurrent()
{

}

void ObjCollection::getNextState(float ts)
{

}

void ObjCollection::getNextRKState(float ts, int idx)
{

}

void ObjCollection::updateState()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->nextState[index].position = glm::vec3(0.0f);
    int count = 0;
    for(int i = 0; i < numChildren; i++)
    {
        if(boop->attribs[childPtrs[i]->getIndex()].id == PARTICLE)
        {
            boop->nextState[index].position += boop->nextState[childPtrs[i]->getIndex()].position;
            count++;
        }
    }
    boop->nextState[index].position = (glm::length(boop->nextState[index].position) / count) * glm::normalize(boop->nextState[index].position);
    boop->nextState[index].velocity = boop->nextState[index].position - boop->curState[index].position;
    boop->curState[index] = boop->nextState[index];
}

void ObjCollection::updateRenderObject()
{

}
