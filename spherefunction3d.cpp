#include "spherefunction3d.h"
#include "imagemanip.h"

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

void SphereFunction3D::setTexNorms(glm::vec3 upVec, glm::vec3 polVec)
{
    normal2 = glm::normalize(upVec);
    normal3 = glm::normalize(polVec);
}

glm::vec4 SphereFunction3D::getTexCol(glm::vec3 pt)
{
    glm::vec3 p0 = origPoint;
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
    float z = glm::dot((pt - p0), -n2) / (normal.x);
    float phi = std::acos(z);
    float x = glm::dot((pt - p0), n0) / (normal.x);
    float y = glm::dot((pt - p0), n1) / (normal.x);
    float cos = std::acos((y / std::sin(phi)));
    if(x < 0) cos = (2 * PI) - cos;
    int buf[4];
    float hold = cos/(2*PI);
    float hold2 = (PI-phi)/PI;

    texture->getDataAt((cos / (2*PI) * texWidth), ((PI - phi) / PI * texHeight), buf);
    return glm::vec4((buf[0] / 255.0f) * cD.a, (buf[1] / 255.0f) * cD.a, (buf[2] / 255.0f) * cD.a, cD.a);
}

glm::vec3 SphereFunction3D::getNMapAt(glm::vec3 pt)
{
    glm::vec3 p0 = origPoint;
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
    float z = glm::dot((pt - p0), -n2) / (normal.x);
    float phi = std::acos(z);
    float x = glm::dot((pt - p0), n0) / (normal.x);
    float y = glm::dot((pt - p0), n1) / (normal.x);
    float cos = std::acos((y / std::sin(phi)));
    if(x < 0) cos = (2 * PI) - cos;
    int buf[4];
    float hold = cos/(2*PI);
    float hold2 = (PI-phi)/PI;

    normMap->getDataAt((cos / (2*PI) * texWidth), ((PI - phi) / PI * texHeight), buf);
    return glm::normalize(glm::vec3((buf[0] / 255.0f), (buf[1] / 255.0f), (buf[2] / 255.0f)));
}

float SphereFunction3D::getBMapAt(glm::vec3 pt)
{
    glm::vec3 p0 = origPoint;
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
    float z = glm::dot((pt - p0), -n2) / (normal.x);
    float phi = std::acos(z);
    float x = glm::dot((pt - p0), n0) / (normal.x);
    float y = glm::dot((pt - p0), n1) / (normal.x);
    float cos = std::acos((y / std::sin(phi)));
    if(x < 0) cos = (2 * PI) - cos;
    int buf[4];
    float hold = cos/(2*PI);
    float hold2 = (PI-phi)/PI;

    bumpMap->getDataAt((cos / (2*PI) * texWidth), ((PI - phi) / PI * texHeight), buf);
    return ((buf[0] / 255.0f) + (buf[1] / 255.0f) + (buf[2] / 255.0f)) / 3;
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

int SphereFunction3D::getRelativeLineMBlur(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    float t;
    float radius = glm::length(normal);
    glm::vec3 curPoint = origPoint + (point2 - origPoint) * tt;
    glm::vec3 length = curPoint - pt;
    float b = glm::dot((curPoint - pt), nL);
    float a = glm::dot(length, length) - b * b;
    float c = glm::dot(curPoint - pt, curPoint - pt) - radius * radius;
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
