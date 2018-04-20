#ifndef PLANEFUNCTION_H
#define PLANEFUNCTION_H

#include "function3d.h"

class PlaneFunction:public Function3D
{
public:
    PlaneFunction();
    PlaneFunction(glm::vec3, glm::vec3);
    glm::vec4 getTexCol(glm::vec3);
    glm::vec3 getNMapAt(glm::vec3);
    float getBMapAt(glm::vec3);
    float getRelativePoint(glm::vec3);
    int getRelativeLine(glm::vec3, glm::vec3, intercept*, int);
    int getRelativeLineMBlur(glm::vec3, glm::vec3, intercept*, int);
    glm::vec3 getSurfaceNormal(glm::vec3);
};

#endif // PLANEFUNCTION_H
