#ifndef LSEGFUNCTION_H
#define LSEGFUNCTION_H

#include "function2d.h"

class LSegFunction: public Function2D
{
public:
    LSegFunction();
    float getRelative(glm::vec2);
    void addPoint(glm::vec2, image*);
    void clearPoints();
    int pointLSeg2D(glm::vec2, glm::vec2, glm::vec2);
};

#endif // LSEGFUNCTION_H
