#ifndef TRIANGLEFUNCTION_H
#define TRIANGLEFUNCTION_H

#include "function3d.h"

class TriangleFunction:public Function3D
{
public:
    TriangleFunction();
    TriangleFunction(glm::vec3, glm::vec3, glm::vec3);
    glm::vec4 getTexCol(glm::vec3 pH);
    glm::vec3 getNMapAt(glm::vec3 pH);
    float getBMapAt(glm::vec3 pH);
    float getRelativePoint(glm::vec3);
    int getRelativeLine(glm::vec3, glm::vec3, intercept*, int);
    glm::vec3 getSurfaceNormal(glm::vec3);

    void getGroupNormal();
    void getGroupPos();
};

#endif // TRIANGLEFUNCTION_H
