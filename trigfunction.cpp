#include "trigfunction.h"

TrigFunction::TrigFunction()
{
}

TrigFunction::TrigFunction(glm::vec2 norm, glm::vec2 pt):Function2D(norm, pt)
{
}

float TrigFunction::getRelative(glm::vec2 pt)
{

    return (std::sin(pt.y / 100) - (pt.x - 1600) / 400);
}

void TrigFunction::setNormal(glm::vec2 n)
{
    normal = n;
}

void TrigFunction::setPoint(glm::vec2 p)
{
    origPoint = p;
}
