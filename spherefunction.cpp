#include "spherefunction.h"

SphereFunction::SphereFunction()
{
}

SphereFunction::SphereFunction(glm::vec2 norm, glm::vec2 pt):Function2D(norm, pt)
{
}

float SphereFunction::getRelative(glm::vec2 pt)
{
    float radius = glm::length(normal);
    float dist = glm::length(origPoint - pt);
    return dist - radius;
}

void SphereFunction::setNormal(glm::vec2 n)
{
    normal = n;
}

void SphereFunction::setPoint(glm::vec2 p)
{
    origPoint = p;
}
