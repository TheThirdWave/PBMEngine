#include "arealight.h"
#include "imagemanip.h"

AreaLight::AreaLight()
{
}

void AreaLight::initialize(glm::vec3 pos, glm::vec3 n, glm::vec3 up, float width, float height, int samples)
{
    position = pos;
    geo.normal = n;
    geo.upVec = up;
    geo.width = width;
    geo.height = height;
    geo.depth = (float) samples;
    type = AREALIGHT;
}

glm::vec3 AreaLight::getRelativeNorm(glm::vec3 pH)
{
    glm::vec3 n0 = glm::normalize(glm::cross(geo.normal, geo.upVec));
    glm::vec3 n1 = glm::normalize(glm::cross(n0, geo.normal));
    glm::vec3 corner = position + n0 * (geo.width * ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f)) + n1 * (geo.height * ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f));
    return glm::normalize(pH - corner);
}

glm::vec4 AreaLight::getColor(glm::vec3 pH)
{
    return cL;
}
