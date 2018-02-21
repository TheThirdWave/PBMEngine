#include "spotlight.h"

SpotLight::SpotLight()
{
}

void SpotLight::initialize(glm::vec3 pos, glm::vec3 n, float rMin, float rMax)
{
    position = pos;
    geo.normal = n;
    geo.radius = rMin;
    geo.width = rMax;
    type = SPOTLIGHT;
}

glm::vec3 SpotLight::getRelativeNorm(glm::vec3 pH)
{
    return glm::normalize(pH - position);
}

