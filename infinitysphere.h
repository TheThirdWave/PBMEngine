#ifndef INFINITYSPHERE_H
#define INFINITYSPHERE_H

#include "function3d.h"

class InfinitySphere:public Function3D
{
public:
    InfinitySphere();
    void setTexNorms(glm::vec3, glm::vec3);
    void setRadius(float);
    glm::vec4 getTexCol(glm::vec3);
    glm::vec3 getNMapAt(glm::vec3);
    float getBMapAt(glm::vec3);
    float getRelativePoint(glm::vec3);
    int getRelativeLine(glm::vec3, glm::vec3, intercept*, int);
    glm::vec3 getSurfaceNormal(glm::vec3);
};

#endif // INFINITYSPHERE_H
