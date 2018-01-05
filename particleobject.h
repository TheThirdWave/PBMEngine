#ifndef PARTICLEOBJECT_H
#define PARTICLEOBJECT_H

#include "physicsobject.h"

class ParticleObject:public PhysicsObject
{
public:
    ParticleObject();
    ParticleObject(void*);
    void initRenderObj(model*);
    void updateRenderObject();
    void setGeometry(glm::vec3);
    void setGeometry(glm::vec3, glm::vec3);
};

#endif // PARTICLEOBJECT_H
