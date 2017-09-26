#ifndef FUNCTION2D_H
#define FUNCTION2D_H

#include "structpile.h"

class Function2D
{
    friend class Imagemanip;
protected:
    glm::vec2 normal;
    glm::vec2 origPoint;
public:
    Function2D();
    Function2D(glm::vec2, glm::vec2);
    void setNormal(glm::vec2);
    void setPoint(glm::vec2);
    virtual float getRelative(glm::vec2) = 0;
};

#endif // FUNCTION2D_H
