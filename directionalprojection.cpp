#include "directionalprojection.h"
#include "imagemanip.h"

DirectionalProjection::DirectionalProjection()
{
}

void DirectionalProjection::initialize(glm::vec3 n, glm::vec3 p)
{
    geo.normal = n;
    position = p;
    type = DIRECTIONAL;
}

glm::vec3 DirectionalProjection::getRelativeNorm(glm::vec3 pH)
{
    return geo.normal;
}

glm::vec4 DirectionalProjection::getColor(glm::vec3 pH)
{
    glm::vec3 p0 = position;
    glm::vec3 up;
    if(geo.normal != glm::vec3(0.0f, 1.0f, 0.0f) && geo.normal != glm::vec3(0.0f, -1.0f, 0.0f)) up = glm::vec3(0.0f, -1.0f, 0.0f);
    else up = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 n0, n1, n2;
    n0 = glm::normalize(glm::cross(geo.normal, up));
    n1 = glm::normalize(glm::cross(geo.normal, n0));
    n2 = geo.normal;
    int texWidth = texture->getWidth();
    int texHeight = texture->getHeight();
    int ptx = glm::dot((pH - p0), n0);
    int pty = glm::dot((pH - p0), n1);
    int buf[4];
    if(ptx < 0 || ptx > texWidth) return glm::vec4(0.0f);
    if(pty < 0 || pty > texHeight) return glm::vec4(0.0f);
    texture->getDataAt(ptx, pty, buf);
    return glm::vec4((buf[0] / 255.0f) * cL.a, (buf[1] / 255.0f) * cL.a, (buf[2] / 255.0f) * cL.a, cL.a);
}

PerspectiveProjection::PerspectiveProjection()
{
}

void PerspectiveProjection::initialize(glm::vec3 n, glm::vec3 p, float f)
{
    geo.normal = n;
    geo.depth = f;
    position = p;
    type = DIRECTIONAL;
}

glm::vec3 PerspectiveProjection::getRelativeNorm(glm::vec3 pH)
{
    return geo.normal;
}

glm::vec4 PerspectiveProjection::getColor(glm::vec3 pH)
{
    glm::vec3 p0 = position;
    glm::vec3 up;
    if(geo.normal != glm::vec3(0.0f, 1.0f, 0.0f) && geo.normal != glm::vec3(0.0f, -1.0f, 0.0f)) up = glm::vec3(0.0f, -1.0f, 0.0f);
    else up = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 n0, n1, n2;
    n0 = glm::normalize(glm::cross(geo.normal, up));
    n1 = glm::normalize(glm::cross(geo.normal, n0));
    n2 = geo.normal;
    float dis = glm::length(glm::dot((pH - p0), n2)) / geo.depth;
    int texWidth = texture->getWidth();
    int texHeight = texture->getHeight();
    int ptx = glm::dot((pH - p0), n0) + (texWidth / 2.0f) ;
    int pty = glm::dot((pH - p0), n1) + (texHeight / 2.0f);
    int buf[4];
    if(ptx < 0 || ptx > texWidth) return glm::vec4(0.0f);
    if(pty < 0 || pty > texHeight) return glm::vec4(0.0f);
    texture->getDataAt(ptx, pty, buf);
    return glm::vec4((buf[0] / 255.0f) * cL.a, (buf[1] / 255.0f) * cL.a, (buf[2] / 255.0f) * cL.a, cL.a);
}
