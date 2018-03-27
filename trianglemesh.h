#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "function3d.h"

class TriangleMesh:public Function3D
{
public:
    TriangleMesh();
    glm::vec4 getTexCol(glm::vec3);
    float getRelativePoint(glm::vec3);
    int getRelativeLine(glm::vec3, glm::vec3, intercept*, int);
    glm::vec3 getSurfaceNormal(glm::vec3);

    void createTetrahedron(float);
};

#endif // TRIANGLEMESH_H
