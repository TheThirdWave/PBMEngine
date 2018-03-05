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
    n1 = glm::normalize(glm::cross(p, n0));
    n2 = up;
    int texWidth = texture->getWidth();
    int texHeight = texture->getHeight();
    int ptx = glm::dot((pt - p0), n0);
    int pty = glm::dot((pt - p0), n1);
    int buf[4];
    ptx = ptx % texWidth;
    if(ptx < 0) ptx += texWidth;
    pty = pty % texHeight;
    if(pty < 0) pty += texHeight;
    texture->getDataAt(ptx, pty, buf);
    return glm::vec4((buf[0] / 255.0f) * cD.a, (buf[1] / 255.0f) * cD.a, (buf[2] / 255.0f) * cD.a, cD.a);
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
