#ifndef QUADRATICFUNCTION3D_H
#define QUADRATICFUNCTION3D_H

#include "function3d.h"

class QuadraticFunction3D:public Function3D
{
public:
    QuadraticFunction3D();
    float getRelativePoint(glm::vec3);
    float getRelativeLine(glm::vec3, glm::vec3);
};

#endif // QUADRATICFUNCTION3D_H
