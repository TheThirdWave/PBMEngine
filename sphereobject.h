#ifndef SPHEREOBJECT_H
#define SPHEREOBJECT_H

#include "physicsobject.h"

class SphereObject: public PhysicsObject
{
public:
    SphereObject();
    void initRenderObj(model*);
    void setGeometry(float);
};

#endif // SPHEREOBJECT_H
