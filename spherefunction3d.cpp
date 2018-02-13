#include "spherefunction3d.h"

SphereFunction3D::SphereFunction3D()
{
}

SphereFunction3D::SphereFunction3D(glm::vec3 pos, float r)
{
    origPoint = pos;
    normal.x = r;
}

void SphereFunction3D::setRadius(float r)
{
    normal.x = r;
}

float SphereFunction3D::getRelativePoint(glm::vec3 pt)
{
    float radius = glm::length(normal);
    float dist = glm::length(origPoint - pt);
    return dist - radius;
}

float SphereFunction3D::getRelativeLine(glm::vec3 pt, glm::vec3 nL)
{
    float t;
    float radius = glm::length(normal);
    float b = glm::dot((origPoint - pt), nL);
    float c = glm::dot(origPoint - pt, origPoint - pt) - radius * radius;
    float delta = b * b - c;
    if(delta > 0 && b > 0)
    {
        float sqrt = std::sqrt(delta);
        t = std::min(b + sqrt, b - sqrt);
        return t;
    }
    t = -1;
    return t;
}
