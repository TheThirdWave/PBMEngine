#ifndef FUNCTION3D_H
#define FUNCTION3D_H

#include "structpile.h"

class Function3D
{
    friend class Imagemanip;
protected:
    glm::vec3 normal, normal2, normal3;
    glm::vec3 origPoint, point2;
    glm::vec3* points[MAX_POINTS];
    glm::vec4 color;
    int a02, a12, a22, a21, a00, pointIdx;
    float s0, s1, s2;
public:
    Function3D();
    Function3D(glm::vec3, glm::vec3);
    Function3D(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
    void setNormal(glm::vec3);
    void setNormal(glm::vec3, glm::vec3);
    void setNormal(glm::vec3, glm::vec3, glm::vec3);
    void setPoint(glm::vec3);
    void setColor(glm::vec4);
    void setQParams(int, int, int, int, int);
    void setQReals(float, float, float);
    virtual float getRelativePoint(glm::vec3) = 0;
    virtual float getRelativeLine(glm::vec3) = 0;
};

#endif // FUNCTION3D_H
