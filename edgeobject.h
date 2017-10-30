#ifndef EDGEOBJECT_H
#define EDGEOBJECT_H

#include "physicsobject.h"

class EdgeObject: public PhysicsObject
{
    friend class PhysicsManager;

public:
    EdgeObject();
    void setRenderObject(RenderObject *);
    void updateRenderObject();
    void getNextState(float);
    void addChild(PhysicsObject *);
    void setSpring(float, float, float);
};

#endif // EDGEOBJECT_H
