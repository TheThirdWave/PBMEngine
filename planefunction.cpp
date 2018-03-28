#include "planefunction.h"
#include "imagemanip.h"

PlaneFunction::PlaneFunction()
{
}

PlaneFunction::PlaneFunction(glm::vec3 pos, glm::vec3 norm)
{
    origPoint = pos;
    normal = glm::normalize(norm);
}

glm::vec4 PlaneFunction::getTexCol(glm::vec3 pt)
{
    glm::vec3 p0 = origPoint;
    glm::vec3 up;
    if(normal != glm::vec3(0.0f, 1.0f, 0.0f) && normal != glm::vec3(0.0f, -1.0f, 0.0f)) up = glm::vec3(0.0f, -1.0f, 0.0f);
    else up = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 n0, n1, n2;
    n0 = glm::normalize(glm::cross(normal, up));
    n1 = glm::normalize(glm::cross(normal, n0));
    n2 = normal;
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

float PlaneFunction::getRelativePoint(glm::vec3 pt)
{
    float dist = glm::dot(pt - origPoint, normal);
    return dist;
}

int PlaneFunction::getRelativeLine(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    float denom = glm::dot(nL, normal);
    if(denom == 0) return idx;

    float t = glm::dot(normal, (origPoint - pt)) / denom;
    if(idx < MAX_LINE_INTERCEPTS && t > 0)
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
