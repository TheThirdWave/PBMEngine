#ifndef SPHEREFUNCTION_H
#define SPHEREFUNCTION_H

#include "function2d.h"

class SphereFunction:public Function2D
{
public:
    SphereFunction();
    SphereFunction(glm::vec2, glm::vec2);
    float getRelative(glm::vec2);
    void setNormal(glm::vec2);
    void setPoint(glm::vec2);
};

#endif // SPHEREFUNCTION_H
