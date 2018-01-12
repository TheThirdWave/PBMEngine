#include "polygonobject.h"
#include "physicsmanager.h"

PolygonObject::PolygonObject()
{
}

PolygonObject::PolygonObject(void* mngr)
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

void PolygonObject::initRenderObj()
{
}

void PolygonObject::addChild(PhysicsObject* c)
{
    if(numChildren < MAX_POLYGON_CHILDREN)
    {
        if(numChildren == 0) childPtrs = new PhysicsObject*[MAX_POLYGON_CHILDREN];
        childPtrs[numChildren++] = c;
    }
    else
    {
        fprintf(stderr, "Polygon cannot take more children.\n");
    }
}

void PolygonObject::setGeometry(glm::vec3 vec, glm::vec3 uv)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].geo.normal = vec;
    boop->attribs[index].geo.upVec = uv;
}

void PolygonObject::updateRenderObject()
{
    geometry hold = getGeometry();
    glm::mat4 rot;
    if(glm::dot(hold.normal, hold.upVec) != -1) rot = glm::orientation(hold.normal, hold.upVec);
    else rot = glm::rotate(glm::mat4(1.0f), (float)PI, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), getPosition());
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * rot * sca);
}

void PolygonObject::getNextChildStates(float ts)
{
    for(int i = 0; i < numChildren; i++)
    {
        childPtrs[i]->getNextState(ts);
    }
}

void PolygonObject::updateChildren()
{
//    for(int i = 0; i < numChildren; i++)
//    {
//        childPtrs[i]->updateState();
  //  }
}

void PolygonObject::setNextFromCurrent()
{
    getNextState(0.0f);
}

void PolygonObject::getNextState(float ts)
{

}

void PolygonObject::getNextRKState(float ts, int idx)
{
    this->getNextState(ts);
}

void PolygonObject::updateState()
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
    int buf[MAX_POLYGON_CHILDREN];
    int numVert = getVertices(buf);
    boop->attribs[index].geo.normal = glm::normalize(glm::cross(childPtrs[buf[0]]->getPosition() - childPtrs[buf[1]]->getPosition(), childPtrs[buf[0]]->getPosition() - childPtrs[buf[2]]->getPosition()));
}
