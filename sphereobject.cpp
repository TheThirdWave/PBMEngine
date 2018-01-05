#include "sphereobject.h"
#include "physicsmanager.h"

SphereObject::SphereObject()
{
}

SphereObject::SphereObject(void* mngr)
{
    manager = mngr;
    scale = glm::vec3(1.0f);
    rendrPtr = NULL;
    numChildren = 0;
    numParents = 0;

    PhysicsManager* boop = (PhysicsManager*) manager;
    index = boop->addPhysObj(this);
    setID(SPHERE);
}

void SphereObject::initRenderObj(model* mdl)
{
    rendrPtr = new RenderObject();
    rendrPtr->setModel(mdl);
}

void SphereObject::setGeometry(float r)
{
    PhysicsManager* boop = (PhysicsManager*) manager;
    boop->attribs[index].geo.radius = r;
}
