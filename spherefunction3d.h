#ifndef SPHEREFUNCTION3D_H
#define SPHEREFUNCTION3D_H

#include "function3d.h"

class SphereFunction3D:public Function3D
{
public:
    SphereFunction3D();
    SphereFunction3D(glm::vec3, float);
    void setRadius(float);
    float getRelativePoint(glm::vec3);
    float getRelativeLine(glm::vec3);
};

#endif // SPHEREFUNCTION3D_H
