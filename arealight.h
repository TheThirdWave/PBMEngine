#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "lightbase.h"

class AreaLight:public LightBase
{
public:
    AreaLight();
    void initialize(glm::vec3 pos, glm::vec3 n, glm::vec3 up, float  width, float height, int samples);
    glm::vec3 getRelativeNorm(glm::vec3 pH);
};

#endif // AREALIGHT_H
