#include "infinitysphere.h"
#include <cfloat>
#include "imagemanip.h"

InfinitySphere::InfinitySphere()
{
}

void InfinitySphere::setRadius(float r)
{
    normal.x = r;
}


void InfinitySphere::setTexNorms(glm::vec3 upVec, glm::vec3 polVec)
{
    normal2 = glm::normalize(upVec);
    normal3 = glm::normalize(polVec);
}

glm::vec4 InfinitySphere::getTexCol(glm::vec3 pt)
{
    float h = glm::length(pt);
    glm::vec3 npt = glm::normalize(pt);
    glm::vec3 up = normal2;
    glm::vec3 forward = normal3;
    if(glm::length(glm::cross(up, forward)) == 0)
    {
        up = glm::vec3(0.0f, -1.0f, 0.0f);
        forward = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    glm::vec3 n0, n1, n2;
    n0 = glm::normalize(glm::cross(up, forward));
    n1 = glm::normalize(glm::cross(up, n0));
    n2 = up;
    int texWidth = texture->getWidth();
    int texHeight = texture->getHeight();
    float z = glm::dot(npt, -n2);
    float phi = std::acos(z);
    float x = glm::dot(npt, n0);
    float y = glm::dot(npt, n1);
    float cos = std::acos((y / std::sin(phi)));
    if(x < 0) cos = (2 * PI) - cos;
    int buf[4];
    texture->getDataAt((cos/(2*PI) * texWidth), ((PI - phi)/PI * texHeight), buf);
    return glm::vec4((buf[0] / 255.0f) * cD.a, (buf[1] / 255.0f) * cD.a, (buf[2] / 255.0f) * cD.a, cD.a);
}

glm::vec3 InfinitySphere::getNMapAt(glm::vec3 pt)
{
    float h = glm::length(pt);
    glm::vec3 npt = glm::normalize(pt);
    glm::vec3 up = normal2;
    glm::vec3 forward = normal3;
    if(glm::length(glm::cross(up, forward)) == 0)
    {
        up = glm::vec3(0.0f, -1.0f, 0.0f);
        forward = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    glm::vec3 n0, n1, n2;
    n0 = glm::normalize(glm::cross(up, forward));
    n1 = glm::normalize(glm::cross(up, n0));
    n2 = up;
    int texWidth = normMap->getWidth();
    int texHeight = normMap->getHeight();
    float z = glm::dot(npt, -n2);
    float phi = std::acos(z);
    float x = glm::dot(npt, n0);
    float y = glm::dot(npt, n1);
    float cos = std::acos((y / std::sin(phi)));
    if(x < 0) cos = (2 * PI) - cos;
    int buf[4];
    normMap->getDataAt((cos/(2*PI) * texWidth), ((PI - phi)/PI * texHeight), buf);
    return glm::normalize(glm::vec3((buf[0] / 255.0f), (buf[1] / 255.0f), (buf[2] / 255.0f)));
}

float InfinitySphere::getBMapAt(glm::vec3 pt)
{
    float h = glm::length(pt);
    glm::vec3 npt = glm::normalize(pt);
    glm::vec3 up = normal2;
    glm::vec3 forward = normal3;
    if(glm::length(glm::cross(up, forward)) == 0)
    {
        up = glm::vec3(0.0f, -1.0f, 0.0f);
        forward = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    glm::vec3 n0, n1, n2;
    n0 = glm::normalize(glm::cross(up, forward));
    n1 = glm::normalize(glm::cross(up, n0));
    n2 = up;
    int texWidth = bumpMap->getWidth();
    int texHeight = bumpMap->getHeight();
    float z = glm::dot(npt, -n2);
    float phi = std::acos(z);
    float x = glm::dot(npt, n0);
    float y = glm::dot(npt, n1);
    float cos = std::acos((y / std::sin(phi)));
    if(x < 0) cos = (2 * PI) - cos;
    int buf[4];
    bumpMap->getDataAt((cos/(2*PI) * texWidth), ((PI - phi)/PI * texHeight), buf);
    return ((buf[0] / 255.0f) + (buf[1] / 255.0f) + (buf[2] / 255.0f)) / 3;
}

float InfinitySphere::getRelativePoint(glm::vec3 pt)
{
    return -normal.x;
}

int InfinitySphere::getRelativeLine(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    hits[idx].t = normal.x;
    hits[idx++].obj = this;
    return idx;
}

glm::vec3 InfinitySphere::getSurfaceNormal(glm::vec3 pt)
{
    return -glm::normalize(pt);
}
