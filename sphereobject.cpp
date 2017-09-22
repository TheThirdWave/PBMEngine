#include "sphereobject.h"

SphereObject::SphereObject()
{
    id = SPHERE;
}

void SphereObject::initRenderObj(model* mdl)
{
    rendrPtr = new RenderObject();
    rendrPtr->setModel(mdl);
}

void SphereObject::setGeometry(float r)
{
    geoDescription.radius = r;
}
