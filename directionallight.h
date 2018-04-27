#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "lightbase.h"

class DirectionalLight:public LightBase
{
public:
    DirectionalLight();
    void initialize(glm::vec3 n);
    glm::vec3 getRelativeNorm(glm::vec3 pH);
    glm::vec4 getColor(glm::vec3 pH);
};

#endif // DIRECTIONALLIGHT_H
