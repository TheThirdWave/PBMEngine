#include "function3d.h"

Function3D::Function3D()
{
    normal = glm::vec3(0.0f);
    origPoint = glm::vec3(0.0f);
}

Function3D::Function3D(glm::vec3 norm, glm::vec3 pt)
{
    normal = norm;
    origPoint = pt;
}

Function3D::Function3D(glm::vec3 norm, glm::vec3 pt, glm::vec3 norm2, glm::vec3 pt2)
{
    normal = norm;
    origPoint = pt;
    normal2 = norm2;
    point2 = pt2;
}

void Function3D::setNormal(glm::vec3 n)
{
    normal = n;
}

void Function3D::setPoint(glm::vec3 p)
{
    origPoint = p;
}

void Function3D::setColor(glm::vec3 col)
{
    color = col;
}

void Function3D::setQParams(int a1, int a2, int a3, int a4, int a5)
{
    a02 = a1;
    a12 = a2;
    a22 = a3;
    a21 = a4;
    a00 = a5;
}

void Function3D::setQReals(int s, int ss)
{
    s0 = s;
    s1 = ss;
}
