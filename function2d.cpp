#include "function2d.h"

Function2D::Function2D()
{
    normal = glm::vec2(0.0f);
    origPoint = glm::vec2(0.0f);
}

Function2D::Function2D(glm::vec2 norm, glm::vec2 pt)
{
    normal = norm;
    origPoint = pt;
}

void Function2D::setNormal(glm::vec2 n)
{
    normal = n;
}

void Function2D::setPoint(glm::vec2 p)
{
    origPoint = p;
}
