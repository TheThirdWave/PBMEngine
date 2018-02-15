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

int PlaneFunction::getRelativeLine(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    float denom = glm::dot(nL, normal);
    if(denom == 0) return idx;

    float t = glm::dot(normal, (pt - origPoint)) / denom;
    if(idx < MAX_LINE_INTERCEPTS && t >= 0)
    {
        hits[idx].t = t;
        hits[idx++].obj = this;
    }

    return idx;
}

glm::vec3 PlaneFunction::getSurfaceNormal(glm::vec3 pt)
{
    return normal;
}
