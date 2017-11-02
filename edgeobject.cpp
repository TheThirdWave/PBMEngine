#include "edgeobject.h"

EdgeObject::EdgeObject()
{
    id = EDGE;
    numChildren = 0;
    childPtrs = new PhysicsObject*[MAX_EDGE_CHILDREN];
}

void EdgeObject::addChild(PhysicsObject* c)
{
    if(numChildren < MAX_EDGE_CHILDREN)
    {
        childPtrs[numChildren++] = c;
    }
    else
    {
        fprintf(stderr, "Edge cannot take more children.\n");
    }
}

void EdgeObject::setSpring(float L, float D, float K)
{
    springL = L;
    springD = D;
    springK = K;
}

void EdgeObject::getNextState(float ts)
{
    newState.position = (childPtrs[0]->getPosition() + childPtrs[1]->getPosition());
    newState.position = (glm::length(newState.position) / 2) * glm::normalize(newState.position);
    newState.velocity = newState.position - curState.position;
}

void EdgeObject::getNextRKState(float ts, int idx)
{
    getNextState(ts);
}

void EdgeObject::getNextChildStates(float ts)
{
    childPtrs[0]->getNextState(ts);
    childPtrs[1]->getNextState(ts);
}

void EdgeObject::updateChildren()
{
    childPtrs[0]->updateState();
    childPtrs[1]->updateState();
}

void EdgeObject::setRenderObject(RenderObject * e)
{

}

void EdgeObject::updateRenderObject()
{

}
