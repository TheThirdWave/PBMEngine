#include "quadraticfunction.h"

QuadraticFunction::QuadraticFunction()
{
}

QuadraticFunction::QuadraticFunction(glm::vec2 norm, glm::vec2 pt):Function2D(norm, pt)
{
}

QuadraticFunction::QuadraticFunction(glm::vec2 norm, glm::vec2 pt, glm::vec2 norm2, glm::vec2 pt2):Function2D(norm, pt, norm2, pt2)
{
}

float QuadraticFunction::getRelative(glm::vec2 pt)
{
    glm::vec2 curVector = pt - origPoint;
    float first = a02*std::pow((glm::dot(normal, curVector)/s0),2) + a11*(glm::dot(normal, curVector)/s1) + a00;
    float second = a12*std::pow((glm::dot(normal2, curVector)/s1),2) + a11*(glm::dot(normal, curVector)/s1) + a00;
    return first + second;
}

void QuadraticFunction::setNormal(glm::vec2 n)
{
    normal = n;
}

void QuadraticFunction::setPoint(glm::vec2 p)
{
    origPoint = p;
}
