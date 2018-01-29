#include "quadraticfunction3d.h"

QuadraticFunction3D::QuadraticFunction3D()
{
}

float QuadraticFunction3D::getRelativePoint(glm::vec3 pt)
{
    glm::vec3 curVector = pt - origPoint;
    float first = a02*std::pow((glm::dot(normal, curVector)/s0),2) + a21*(glm::dot(normal, curVector)/s2) + a00;
    float second = a12*std::pow((glm::dot(normal2, curVector)/s1),2) + a21*(glm::dot(normal, curVector)/s2) + a00;
    float third = a22*std::pow((glm::dot(normal3, curVector)/s2),2) + a21*(glm::dot(normal, curVector)/s2) + a00;
    return first + second + third;
}

float QuadraticFunction3D::getRelativeLine(glm::vec3 pt)
{

}
