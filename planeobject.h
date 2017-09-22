#ifndef PLANEOBJECT_H
#define PLANEOBJECT_H

#include "physicsobject.h"

class PlaneObject: public PhysicsObject
{
public:
    PlaneObject();
    void initRenderObj();
    void setGeometry(glm::vec3);
    void updateRenderObject();
    void updateState();
};

#endif // PLANEOBJECT_H
