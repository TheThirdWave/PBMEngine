#ifndef TRIGFUNCTION_H
#define TRIGFUNCTION_H

#include "function2d.h"

class TrigFunction:public Function2D
{
public:
    TrigFunction();
    TrigFunction(glm::vec2, glm::vec2);
    float getRelative(glm::vec2);
    void setNormal(glm::vec2);
    void setPoint(glm::vec2);
};

#endif // TRIGFUNCTION_H
