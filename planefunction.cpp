#include "planefunction.h"

PlaneFunction::PlaneFunction()
{
}

PlaneFunction::PlaneFunction(glm::vec3 pos, glm::vec3 norm)
{
    origPoint = pos;
    normal = glm::normalize(norm);
}

float PlaneFunction::getRelativePoint(glm::vec3 pt)
{
    float dist = glm::dot(pt - origPoint, normal);
    return dist;
}

float PlaneFunction::getRelativeLine(glm::vec3 pt, glm::vec3 nL)
{
    float denom = glm::dot(nL, normal);
    if(denom == 0)
    {
        return -1;
    }
    else
    {
        return glm::dot(normal, (pt - origPoint)) / denom;
    }
}
