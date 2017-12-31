#include "polygonobject.h"

PolygonObject::PolygonObject()
{
    id = POLYGON;
    alive = true;
    active = true;
    numChildren = 0;
    childPtrs = new PhysicsObject*[MAX_POLYGON_CHILDREN];
}

void PolygonObject::initRenderObj()
{
}

void PolygonObject::addChild(PhysicsObject* c)
{
    if(numChildren < MAX_POLYGON_CHILDREN)
    {
        childPtrs[numChildren++] = c;
    }
    else
    {
        fprintf(stderr, "Polygon cannot take more children.\n");
    }
}

void PolygonObject::setGeometry(glm::vec3 vec, glm::vec3 uv)
{
    geoDescription.normal = vec;
    geoDescription.upVec = uv;
}

void PolygonObject::updateRenderObject()
{
    glm::mat4 rot;
    if(glm::dot(geoDescription.normal, geoDescription.upVec) != -1) rot = glm::orientation(geoDescription.normal, geoDescription.upVec);
    else rot = glm::rotate(glm::mat4(1.0f), (float)PI, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), curState.position);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * rot * sca);
}

void PolygonObject::getNextChildStates(float ts)
{
    //for(int i = 0; i < numChildren; i++)
    //{
//        childPtrs[i]->getNextState(ts);
//    }
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
    newState.position = glm::vec3(0.0f);
    int count = 0;
    for(int i = 0; i < numChildren; i++)
    {
        if(childPtrs[i]->getId() == 3)
        {
            newState.position += childPtrs[i]->getPosition();
            count++;
        }
    }
    newState.position = (glm::length(newState.position) / count) * glm::normalize(newState.position);
    newState.velocity = newState.position - curState.position;
}

void PolygonObject::getNextRKState(float ts, int idx)
{
    this->getNextState(ts);
}

void PolygonObject::updateState()
{
    curState = newState;
    int buf[MAX_POLYGON_CHILDREN];
    int numVert = getVertices(buf);
    geoDescription.normal = glm::normalize(glm::cross(childPtrs[buf[0]]->getPosition() - childPtrs[buf[1]]->getPosition(), childPtrs[buf[0]]->getPosition() - childPtrs[buf[2]]->getPosition()));
}
