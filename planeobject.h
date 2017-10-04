#ifndef PLANEOBJECT_H
#define PLANEOBJECT_H

#include "physicsobject.h"

class PlaneObject: public PhysicsObject
{
    friend class PhysicsManager;
public:
    PlaneObject();
    void initRenderObj();
    void setGeometry(glm::vec3, glm::vec3);
    void updateRenderObject();
    void getNextState(float);
    void updateState();
};

#endif // PLANEOBJECT_H
