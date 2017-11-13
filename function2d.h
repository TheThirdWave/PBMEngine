#ifndef FUNCTION2D_H
#define FUNCTION2D_H

#include "structpile.h"

class Function2D
{
    friend class Imagemanip;
protected:
    glm::vec2 normal, normal2;
    glm::vec2 origPoint, point2;
    glm::vec2* points[MAX_POINTS];
    int a02, a12, a11, a00, s0, s1, pointIdx;
public:
    Function2D();
    Function2D(glm::vec2, glm::vec2);
    Function2D(glm::vec2, glm::vec2, glm::vec2, glm::vec2);
    void setNormal(glm::vec2);
    void setPoint(glm::vec2);
    void setQParams(int, int, int, int);
    void setQReals(int, int);
    virtual float getRelative(glm::vec2) = 0;
};

#endif // FUNCTION2D_H
