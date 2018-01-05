#include "planeobject.h"
#include "physicsmanager.h"

PlaneObject::PlaneObject()
{
}

PlaneObject::PlaneObject(void* mngr)
{
    manager = mngr;
    scale = glm::vec3(1.0f);
    rendrPtr = NULL;
    numChildren = 0;
    numParents = 0;

    PhysicsManager* boop = (PhysicsManager*) manager;
    index = boop->addPhysObj(this);
    setID(PLANE);
}

void PlaneObject::initRenderObj()
{
}

void PlaneObject::setGeometry(glm::vec3 vec, glm::vec3 uv)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].geo.normal = vec;
    boop->attribs[index].geo.upVec = uv;
}

void PlaneObject::updateRenderObject()
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    glm::mat4 rot;
    if(glm::dot(boop->attribs[index].geo.normal, boop->attribs[index].geo.upVec) != -1) rot = glm::orientation(boop->attribs[index].geo.normal, boop->attribs[index].geo.upVec);
    else rot = glm::rotate(glm::mat4(1.0f), (float)PI, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), boop->curState[index].position);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * rot * sca);
}

void PlaneObject::getNextState(float t)
{

}

void PlaneObject::updateState(){

}
