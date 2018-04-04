#include "trianglefunction.h"
#include "imagemanip.h"

TriangleFunction::TriangleFunction()
{
    parent = NULL;
}

TriangleFunction::TriangleFunction(glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
    tri.a = A;
    tri.b = B;
    tri.c = C;
    origPoint = (A + B + C) / 3.0f;
    normal = glm::normalize(glm::cross(C - A, B - A));
    parent = NULL;
}

glm::vec4 TriangleFunction::getTexCol(glm::vec3 pt)
{
    glm::vec3 hitPoint = pt;
    glm::vec3 e01 = tri.b - tri.a;
    glm::vec3 e12 = tri.c - tri.b;
    glm::vec3 e20 = tri.a - tri.c;
    glm::vec3 p1x = hitPoint - tri.b;
    glm::vec3 p2x = hitPoint - tri.c;
    glm::vec3 Vn = glm::cross(e01, e12);
    float area2 = glm::length(Vn);
    glm::vec3 normal = Vn/area2;
    float w = glm::dot(glm::cross(e12, p1x), normal) / area2;
    float u = glm::dot(glm::cross(e20, p2x), normal) / area2;
    float v = 1 - u - w;
    glm::vec2 texCoord = w * uvs.a + u * uvs.b + v * uvs.c;

    int texWidth = texture->getWidth();
    int texHeight = texture->getHeight();
    int buf[4];
    texture->getDataAt(texCoord.x * texWidth, texCoord.y * texHeight, buf);
    return glm::vec4((buf[0] / 255.0f) * cD.a, (buf[1] / 255.0f) * cD.a, (buf[2] / 255.0f) * cD.a, cD.a);

}

float TriangleFunction::getRelativePoint(glm::vec3 pt)
{
    float dist = glm::dot(pt - origPoint, normal);

    /*glm::vec3 closestPoint = pt - (normal * dist);


    //Get the barycentric coordinates of the point of collision on the plane. (We're assuming that the "polygon" is a triangle here)
    glm::vec3 e01 = tri.b - tri.a;
    glm::vec3 e12 = tri.c - tri.b;
    glm::vec3 e20 = tri.a - tri.c;
    glm::vec3 p1x = closestPoint - tri.b;
    glm::vec3 p2x = closestPoint - tri.c;
    glm::vec3 Vn = glm::cross(e01, e12);
    float area2 = glm::length(Vn);
    glm::vec3 n2 = Vn/area2;
    float w = glm::dot(glm::cross(e12, p1x), n2) / area2;
    float u = glm::dot(glm::cross(e20, p2x), n2) / area2;
    float v = 1 - u - w;
    if(w < 0 || u < 0 || v < 0)
    {
        dist = glm::length(pt - ((tri.a + tri.b + tri.c) / 3.0f));
    }*/


    return dist;
}

int TriangleFunction::getRelativeLine(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    glm::vec3 intNorm = glm::normalize(glm::cross(tri.b - tri.a, tri.c - tri.a));
    float denom = glm::dot(nL, intNorm);
    if(denom == 0) return idx;

    //first get the point where the line intercepts the plane.
    float t = glm::dot(intNorm, (origPoint - pt)) / denom;
    glm::vec3 hitPoint = pt + nL * t;

    //if t is positive test if the plane intercept is within the triangle.
    if(idx < MAX_LINE_INTERCEPTS && t > 0)
    {
    //Get the barycentric coordinates of the point of collision on the plane. (We're assuming that the "polygon" is a triangle here)
    glm::vec3 e01 = tri.b - tri.a;
    glm::vec3 e12 = tri.c - tri.b;
    glm::vec3 e20 = tri.a - tri.c;
    glm::vec3 p1x = hitPoint - tri.b;
    glm::vec3 p2x = hitPoint - tri.c;
    glm::vec3 Vn = glm::cross(e01, e12);
    float area2 = glm::length(Vn);
    glm::vec3 n2 = Vn/area2;
    float w = glm::dot(glm::cross(e12, p1x), n2) / area2;
    float u = glm::dot(glm::cross(e20, p2x), n2) / area2;
    float v = 1 - u - w;

    //If the barycentric coordinates are all between 0 and 1, the collision point is inside the triangle, otherwise it's a miss.
    if(u < 0 || v < 0 || w < 0)
    {
        return idx;
    }

    hits[idx].t = t;
    hits[idx++].obj = this;
    }

    return idx;
}

glm::vec3 TriangleFunction::getSurfaceNormal(glm::vec3 pt)
{
    /*glm::vec3 hitPoint = pt;
    glm::vec3 e01 = tri.b - tri.a;
    glm::vec3 e12 = tri.c - tri.b;
    glm::vec3 e20 = tri.a - tri.c;
    glm::vec3 p1x = hitPoint - tri.b;
    glm::vec3 p2x = hitPoint - tri.c;
    glm::vec3 Vn = glm::cross(e01, e12);
    float area2 = glm::length(Vn);
    glm::vec3 normal = Vn/area2;
    float w = glm::dot(glm::cross(e12, p1x), normal) / area2;
    float u = glm::dot(glm::cross(e20, p2x), normal) / area2;
    float v = 1 - u - w;
    normal = w * nTri.a + u * nTri.b + v * nTri.c;*/
    //return glm::normalize(glm::cross(tri.c - tri.a, tri.b - tri.a));
    return normal;
}

void TriangleFunction::getGroupNormal()
{
    normal = glm::normalize((nTri.a + nTri.b + nTri.c) / 3.0f);
}

void TriangleFunction::getGroupPos()
{
    origPoint = (tri.a + tri.b + tri.c) / 3.0f;
}
