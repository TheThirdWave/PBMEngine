#include "pointlight.h"

PointLight::PointLight()
{
}

void PointLight::initialize(glm::vec3 pos)
{
    position = pos;
    type = POINT;
}

glm::vec3 PointLight::getRelativeNorm(glm::vec3 pH)
{
    return glm::normalize(pH - position);
}

glm::vec4 PointLight::getColor(glm::vec3 pH)
{
    return cL;
}
