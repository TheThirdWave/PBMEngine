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

void Function3D::setNormal(glm::vec3 n, glm::vec3 n2)
{
    normal = n;
    normal2 = n2;
}

void Function3D::setNormal(glm::vec3 n, glm::vec3 n2, glm::vec3 n3)
{
    normal = glm::normalize(n);
    normal2 = glm::normalize(n2);
    normal3 = glm::normalize(n3);
}

void Function3D::setPoint(glm::vec3 p)
{
    origPoint = p;
}

void Function3D::setColor(glm::vec4 cs, glm::vec4 cd, glm::vec4 ca)
{
    cS = cs;
    cD = cd;
    cA = ca;
}

void Function3D::setQParams(int a1, int a2, int a3, int a4, int a5)
{
    a02 = a1;
    a12 = a2;
    a22 = a3;
    a21 = a4;
    a00 = a5;
}

void Function3D::setQReals(float s, float ss, float sss)
{
    s0 = s;
    s1 = ss;
    s2 = sss;
}

void Function3D::setDisp(float d)
{
    disp = d;
}

void Function3D::setGeometry(geometry g)
{
    geo = g;
}

void Function3D::setTexture(Imagemanip* tex)
{
    texture = tex;
}

glm::vec4 Function3D::getCS()
{
    return cS;
}

glm::vec4 Function3D::getCD()
{
    return cD;
}

glm::vec4 Function3D::getCA()
{
    return cA;
}

float Function3D::getDisp()
{
    return disp;
}

geometry Function3D::getGeo()
{
    return geo;
}
