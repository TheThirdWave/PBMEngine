#include "polygonobject.h"

PolygonObject::PolygonObject()
{
    id = POLYGON;
}

void PolygonObject::initRenderObj()
{
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
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * rot * sca);
}

void PolygonObject::updateState(){

}
