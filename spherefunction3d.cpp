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

float SphereFunction3D::getRelativeLine(glm::vec3 pt)
{

}
