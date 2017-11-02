#ifndef POLYGONOBJECT_H
#define POLYGONOBJECT_H

#include "physicsobject.h"

class PolygonObject:public PhysicsObject
{
public:
    PolygonObject();
    void initRenderObj();
    void setGeometry(glm::vec3, glm::vec3);
    void addChild(PhysicsObject*);
    void getNextChildStates(float);
    void updateChildren();
    void updateRenderObject();
    void getNextState(float);
    void updateState();
};

#endif // POLYGONOBJECT_H
