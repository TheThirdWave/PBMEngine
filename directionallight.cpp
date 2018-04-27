#include "directionallight.h"
#include "imagemanip.h"

DirectionalLight::DirectionalLight()
{
}

void DirectionalLight::initialize(glm::vec3 n)
{
    geo.normal = n;
    type = DIRECTIONAL;
}

glm::vec3 DirectionalLight::getRelativeNorm(glm::vec3 pH)
{
    return geo.normal;
}

glm::vec4 DirectionalLight::getColor(glm::vec3 pH)
{
    return cL;
}
