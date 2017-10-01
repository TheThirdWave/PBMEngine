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

Function2D::Function2D(glm::vec2 norm, glm::vec2 pt, glm::vec2 norm2, glm::vec2 pt2)
{
    normal = norm;
    origPoint = pt;
    normal2 = norm2;
    point2 = pt2;
}

void Function2D::setNormal(glm::vec2 n)
{
    normal = n;
}

void Function2D::setPoint(glm::vec2 p)
{
    origPoint = p;
}

void Function2D::setQParams(int a1, int a2, int a3, int a4)
{
    a02 = a1;
    a12 = a2;
    a11 = a3;
    a00 = a4;
}

void Function2D::setQReals(int s, int ss)
{
    s0 = s;
    s1 = ss;
}
