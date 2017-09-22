#include "planeobject.h"

PlaneObject::PlaneObject()
{
    id = PLANE;
}

void PlaneObject::initRenderObj()
{
}

void PlaneObject::setGeometry(glm::vec3 vec)
{
    geoDescription.normal = vec;
}

void PlaneObject::updateRenderObject()
{
    glm::mat4 rot = glm::orientation(geoDescription.normal, glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), position);

    rendrPtr->setPosMatrix(pos * rot);
}

void PlaneObject::updateState(){

}
