#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "lightbase.h"

class PointLight:public LightBase
{
public:
    PointLight();
    void initialize(glm::vec3 pos);
    glm::vec3 getRelativeNorm(glm::vec3 pH);
};

#endif // POINTLIGHT_H
