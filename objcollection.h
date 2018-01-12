#ifndef OBJCOLLECTION_H
#define OBJCOLLECTION_H

#include "physicsobject.h"

class ObjCollection:public PhysicsObject
{
    friend class PhysicsManager;
public:
    ObjCollection();
    ObjCollection(void*);
    void initRenderObj();
    void getNextState(float);
    void getNextRKState(float, int);
    void setNextFromCurrent();
    void updateState();
    void updateRenderObject();
};

#endif // OBJCOLLECTION_H
