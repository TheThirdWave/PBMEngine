#include "polygonobject.h"

PolygonObject::PolygonObject()
{
    id = POLYGON;
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
    for(int i = 0; i < numChildren; i++)
    {
        childPtrs[i]->getNextState(ts);
    }
}

void PolygonObject::updateChildren()
{
    for(int i = 0; i < numChildren; i++)
    {
        childPtrs[i]->updateState();
    }
}

void PolygonObject::getNextState(float ts)
{
    newState.position = glm::vec3(0.0f);
    for(int i = 0; i < numChildren; i++) newState.position += childPtrs[i]->getPosition();
    newState.position = (glm::length(newState.position) / numChildren) * glm::normalize(newState.position);
    newState.velocity = newState.position - curState.position;
}

void PolygonObject::getNextRKState(float ts, int idx)
{
    getNextState(ts);
}

void PolygonObject::updateState()
{
    curState = newState;
    geoDescription.normal = glm::normalize(glm::cross(childPtrs[0]->getPosition() - childPtrs[1]->getPosition(), childPtrs[0]->getPosition() - childPtrs[2]->getPosition()));
}
