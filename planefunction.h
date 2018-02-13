#ifndef PLANEFUNCTION_H
#define PLANEFUNCTION_H

#include "function3d.h"

class PlaneFunction:public Function3D
{
public:
    PlaneFunction();
    PlaneFunction(glm::vec3, glm::vec3);
    float getRelativePoint(glm::vec3);
    float getRelativeLine(glm::vec3, glm::vec3);
};

#endif // PLANEFUNCTION_H
