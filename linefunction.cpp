#include "linefunction.h"

LineFunction::LineFunction()
{
}

LineFunction::LineFunction(glm::vec2 norm, glm::vec2 pt):Function2D(norm, pt)
{
}

float LineFunction::getRelative(glm::vec2 pt)
{
    return glm::dot(normal, (pt - origPoint));
}

void LineFunction::setNormal(glm::vec2 n)
{
    normal = n;
}

void LineFunction::setPoint(glm::vec2 p)
{
    origPoint = p;
}
