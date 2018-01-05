#ifndef EDGEOBJECT_H
#define EDGEOBJECT_H

#include "physicsobject.h"

class EdgeObject: public PhysicsObject
{
    friend class PhysicsManager;

public:
    EdgeObject();
    EdgeObject(void*);
    void setRenderObject(RenderObject *);
    void updateRenderObject();
    void getNextState(float);
    void getNextChildStates(float);
    void getNextRKState(float, int);
    void updateChildren();
    void updateState();
    void addChild(PhysicsObject *);
    void setSpring(float, float, float);
};

#endif // EDGEOBJECT_H
