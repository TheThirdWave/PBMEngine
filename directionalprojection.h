#ifndef DIRECTIONALPROJECTION_H
#define DIRECTIONALPROJECTION_H

#include "lightbase.h"

class DirectionalProjection:public LightBase
{
public:
    DirectionalProjection();
    void initialize(glm::vec3 n, glm::vec3 p);
    glm::vec3 getRelativeNorm(glm::vec3 pH);
    glm::vec4 getColor(glm::vec3 pH);
};

class PerspectiveProjection:public LightBase
{
public:
    PerspectiveProjection();
    void initialize(glm::vec3 n, glm::vec3 p, float f);
    glm::vec3 getRelativeNorm(glm::vec3 pH);
    glm::vec4 getColor(glm::vec3 pH);
};

#endif // DIRECTIONALPROJECTION_H
