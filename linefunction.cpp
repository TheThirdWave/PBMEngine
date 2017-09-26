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
