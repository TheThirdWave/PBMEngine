#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "lightbase.h"

class SpotLight:public LightBase
{
public:
    SpotLight();
    void initialize(glm::vec3 pos, glm::vec3 n, float rMin, float rMax);
    glm::vec3 getRelativeNorm(glm::vec3 pH);
};

#endif // SPOTLIGHT_H
