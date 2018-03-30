#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "function3d.h"
#include "structpile.h"

class TriangleMesh:public Function3D
{
public:
    TriangleMesh();
    glm::vec4 getTexCol(glm::vec3 pH);
    float getRelativePoint(glm::vec3 pH);
    int getRelativeLine(glm::vec3, glm::vec3, intercept*, int);
    glm::vec3 getSurfaceNormal(glm::vec3 pH);

    intercept getClosest(glm::vec3 pH);
    void createTetrahedron(float);
    void createCube(float);
    void loadFromModel(model *, float);
};

#endif // TRIANGLEMESH_H
