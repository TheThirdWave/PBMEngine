#ifndef SPHEREOBJECT_H
#define SPHEREOBJECT_H

#include "physicsobject.h"

class SphereObject: public PhysicsObject
{
public:
    SphereObject();
    SphereObject(void*);
    void initRenderObj(model*);
    void setGeometry(float);
};

#endif // SPHEREOBJECT_H
