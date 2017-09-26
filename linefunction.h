#ifndef LINEFUNCTION_H
#define LINEFUNCTION_H

#include "function2d.h"

class LineFunction: public Function2D
{
public:
    LineFunction();
    LineFunction(glm::vec2, glm::vec2);
    float getRelative(glm::vec2);
};

#endif // LINEFUNCTION_H
