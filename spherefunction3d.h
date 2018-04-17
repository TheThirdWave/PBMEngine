#ifndef SPHEREFUNCTION3D_H
#define SPHEREFUNCTION3D_H

#include "function3d.h"

class SphereFunction3D:public Function3D
{
public:
    SphereFunction3D();
    SphereFunction3D(glm::vec3, float);
    void setRadius(float);
    void setTexNorms(glm::vec3, glm::vec3);
    glm::vec4 getTexCol(glm::vec3);
    glm::vec3 getNMapAt(glm::vec3);
    float getBMapAt(glm::vec3);
    float getRelativePoint(glm::vec3);
    int getRelativeLine(glm::vec3, glm::vec3, intercept*, int);
    int getRelativeLineMBlur(glm::vec3, glm::vec3, intercept*, int);
    glm::vec3 getSurfaceNormal(glm::vec3);
};

#endif // SPHEREFUNCTION3D_H
