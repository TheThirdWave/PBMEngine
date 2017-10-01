#ifndef QUADRATICFUNCTION_H
#define QUADRATICFUNCTION_H

#include "function2d.h"

class QuadraticFunction:public Function2D
{
public:
    QuadraticFunction();
    QuadraticFunction(glm::vec2, glm::vec2);
    QuadraticFunction(glm::vec2, glm::vec2, glm::vec2, glm::vec2);
    float getRelative(glm::vec2);
    void setNormal(glm::vec2);
    void setPoint(glm::vec2);
};

#endif // QUADRATICFUNCTION_H
