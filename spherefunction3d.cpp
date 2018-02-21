#include "spherefunction3d.h"

SphereFunction3D::SphereFunction3D()
{
}

SphereFunction3D::SphereFunction3D(glm::vec3 pos, float r)
{
    origPoint = pos;
    normal.x = r;
}

void SphereFunction3D::setRadius(float r)
{
    normal.x = r;
}

float SphereFunction3D::getRelativePoint(glm::vec3 pt)
{
    float radius = glm::length(normal);
    float dist = glm::length(origPoint - pt);
    return dist - radius;
}

int SphereFunction3D::getRelativeLine(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    float t;
    float radius = glm::length(normal);
    glm::vec3 length = origPoint - pt;
    float b = glm::dot((origPoint - pt), nL);
    float a = glm::dot(length, length) - b * b;
    float c = glm::dot(origPoint - pt, origPoint - pt) - radius * radius;
    float delta = b * b - c;
    if(delta > 0)
    {
        float sqrt = std::sqrt(delta);
        t = b + sqrt;
        if(idx < MAX_LINE_INTERCEPTS && t >= 0)
        {
            hits[idx].t = t;
            hits[idx++].obj = this;
        }
        t = b - sqrt;
        if(idx < MAX_LINE_INTERCEPTS && t >= 0)
        {
            hits[idx].t = t;
            hits[idx++].obj = this;
        }
        return idx;
    }
    t = -1;
    return idx;
}

glm::vec3 SphereFunction3D::getSurfaceNormal(glm::vec3 pt)
{
    return glm::normalize(pt - origPoint);
}
