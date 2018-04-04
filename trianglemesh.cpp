#include "trianglemesh.h"
#include "../glm-0.9.8.5/glm/gtx/rotate_vector.hpp"
#include "trianglefunction.h"

TriangleMesh::TriangleMesh()
{
}

glm::vec4 TriangleMesh::getTexCol(glm::vec3 pH)
{
    intercept closest = getClosest(pH);
    return closest.obj->getTexCol(pH);
}

float TriangleMesh::getRelativePoint(glm::vec3 pH)
{
    float dist;
    for(int i = 0; i < numChildren; i++)
    {
        if(i == 0) dist = children[i].getRelativePoint(pH);
        else
        {
            dist = std::min(children[i].getRelativePoint(pH), dist);
        }
    }
    return dist;
}

int TriangleMesh::getRelativeLine(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    int holdIdx = idx;
    for(int i = 0; i < numChildren; i++)
    {
        idx = children[i].getRelativeLine(pt, nL, hits, idx);
        if(idx > holdIdx)
        {
            int x = 0;
            //hits[holdIdx++].obj = this;
        }
    }
    return idx;
}

glm::vec3 TriangleMesh::getSurfaceNormal(glm::vec3 pH)
{
    intercept closest = getClosest(pH);
    return closest.obj->getSurfaceNormal(pH);
}

intercept TriangleMesh::getClosest(glm::vec3 pH)
{
    intercept closest;
    float hold;
    for(int i = 0; i < numChildren; i++)
    {
        hold = children[i].getRelativePoint(pH);
        if(i == 0)
        {
            closest.t = hold;
            closest.obj = &children[i];
        }
        else if(hold < closest.t)
        {
            closest.t = hold;
            closest.obj = &children[i];
        }
    }
    return closest;
}

void TriangleMesh::createTetrahedron(float radius)
{
    glm::vec3 upVec = normal;
    glm::vec3 n0 = glm::normalize(glm::cross(normal, normal2));
    glm::vec3 points[4];
    points[0] = origPoint + n0 * radius;
    glm::vec3 n1 = glm::rotate(n0, (float)(2*PI/3), upVec);
    points[1] = origPoint + n1 * radius;
    points[2] = origPoint + glm::rotate(n1, (float)(2*PI/3), upVec) * radius;
    points[3] = origPoint + upVec * radius;
    glm::vec3 center = (points[0] + points[1] + points[2] + points[3]) / 4.0f;
    glm::vec3 norms[4];
    for(int i = 0; i < 4; i++)
    {
        norms[i] = glm::normalize(points[i] - center);
    }
    TriangleFunction* triangles = new TriangleFunction[4];
    for(int i = 0; i < 4; i ++)
    {
        triangles[i].setColor(cS, cD, cA);
        triangles[i].setGeometry(geo);
        triangles[i].setTexture(texture);
        triangles[i].shader = shader;
    }
    triangles[0].setTriangle(points[0], points[1], points[2]);
    triangles[0].setTriNorms(norms[0], norms[1], norms[2]);
    triangles[0].getGroupNormal();
    triangles[0].getGroupPos();
    triangles[1].setTriangle(points[3], points[0], points[1]);
    triangles[1].setTriNorms(norms[3], norms[0], norms[1]);
    triangles[1].getGroupNormal();
    triangles[1].getGroupPos();
    triangles[2].setTriangle(points[3], points[1], points[2]);
    triangles[2].setTriNorms(norms[3], norms[1], norms[2]);
    triangles[2].getGroupNormal();
    triangles[2].getGroupPos();
    triangles[3].setTriangle(points[3], points[2], points[0]);
    triangles[3].setTriNorms(norms[3], norms[2], norms[0]);
    triangles[3].getGroupNormal();
    triangles[3].getGroupPos();

    setChildren(triangles, 4);
}

void TriangleMesh::createCube(float radius)
{
    glm::vec3 upVec = normal;
    glm::vec3 n0 = glm::normalize(glm::cross(normal, normal2));
    glm::vec3 n1 = glm::normalize(glm::cross(upVec, n0));
    glm::vec3 points[8];
    glm::vec3 norms[8];
    TriangleFunction* triangles = new TriangleFunction[12];
    glm::vec3 rotPtr = glm::rotate(glm::rotate(n0, (float)(2*PI/8), n1), (float)(2*PI/8), upVec);
    points[0] = origPoint + rotPtr * radius;
    rotPtr = glm::rotate(rotPtr, (float)(2*PI/4), upVec);
    points[1] = origPoint + rotPtr * radius;
    rotPtr = glm::rotate(rotPtr, (float)(2*PI/4), upVec);
    points[2] = origPoint + rotPtr * radius;
    rotPtr = glm::rotate(rotPtr, (float)(2*PI/4), upVec);
    points[3] = origPoint + rotPtr * radius;
    rotPtr = glm::rotate(glm::rotate(n0, -(float)(2*PI/8), n1), (float)(2*PI/8), upVec);
    points[4] = origPoint + rotPtr * radius;
    rotPtr = glm::rotate(rotPtr, (float)(2*PI/4), upVec);
    points[5] = origPoint + rotPtr * radius;
    rotPtr = glm::rotate(rotPtr, (float)(2*PI/4), upVec);
    points[6] = origPoint + rotPtr * radius;
    rotPtr = glm::rotate(rotPtr, (float)(2*PI/4), upVec);
    points[7] = origPoint + rotPtr * radius;
    for(int i = 0; i < 8; i++)
    {
        norms[i] = glm::normalize(points[i] - origPoint);
    }
    for(int i = 0; i < 12; i++)
    {
        triangles[i].setColor(cS, cD, cA);
        triangles[i].setGeometry(geo);
        triangles[i].shader = shader;
        triangles[i].setTexture(texture);
    }
    triangles[0].setTriangle(points[0], points[2], points[1]);
    triangles[0].setTriNorms(norms[0], norms[2], norms[1]);
    triangles[0].getGroupNormal();
    triangles[0].getGroupPos();
    triangles[1].setTriangle(points[2], points[3], points[0]);
    triangles[1].setTriNorms(norms[2], norms[3], norms[0]);
    triangles[1].getGroupNormal();
    triangles[1].getGroupPos();
    triangles[2].setTriangle(points[0], points[4], points[5]);
    triangles[2].setTriNorms(norms[0], norms[4], norms[5]);
    triangles[2].getGroupNormal();
    triangles[2].getGroupPos();
    triangles[3].setTriangle(points[5], points[1], points[0]);
    triangles[3].setTriNorms(norms[5], norms[1], norms[0]);
    triangles[3].getGroupNormal();
    triangles[3].getGroupPos();
    triangles[4].setTriangle(points[1], points[5], points[6]);
    triangles[4].setTriNorms(norms[1], norms[5], norms[6]);
    triangles[4].getGroupNormal();
    triangles[4].getGroupPos();
    triangles[5].setTriangle(points[6], points[2], points[1]);
    triangles[5].setTriNorms(norms[6], norms[2], norms[1]);
    triangles[5].getGroupNormal();
    triangles[5].getGroupPos();
    triangles[6].setTriangle(points[2], points[6], points[7]);
    triangles[6].setTriNorms(norms[2], norms[6], norms[7]);
    triangles[6].getGroupNormal();
    triangles[6].getGroupPos();
    triangles[7].setTriangle(points[7], points[3], points[2]);
    triangles[7].setTriNorms(norms[7], norms[3], norms[2]);
    triangles[7].getGroupNormal();
    triangles[7].getGroupPos();
    triangles[8].setTriangle(points[3], points[7], points[4]);
    triangles[8].setTriNorms(norms[3], norms[7], norms[4]);
    triangles[8].getGroupNormal();
    triangles[8].getGroupPos();
    triangles[9].setTriangle(points[4], points[0], points[3]);
    triangles[9].setTriNorms(norms[4], norms[0], norms[3]);
    triangles[9].getGroupNormal();
    triangles[9].getGroupPos();
    triangles[10].setTriangle(points[4], points[6], points[5]);
    triangles[10].setTriNorms(norms[4], norms[6], norms[5]);
    triangles[10].getGroupNormal();
    triangles[10].getGroupPos();
    triangles[11].setTriangle(points[6], points[4], points[7]);
    triangles[11].setTriNorms(norms[6], norms[4], norms[7]);
    triangles[11].getGroupNormal();
    triangles[11].getGroupPos();

    setChildren(triangles, 12);
}

void TriangleMesh::loadFromModel(model* mod, float scale)
{
    glm::vec3 upVec = normal;
    glm::vec3 n0 = glm::normalize(glm::cross(normal, normal2));
    glm::vec3 n1 = glm::normalize(glm::cross(upVec, n0));
    TriangleFunction* triangles = new TriangleFunction[mod->idxLen / 3];
    for(int i = 0; i < mod->idxLen / 3; i++)
    {
        triangle vTri, nTri;
        triangle2d tTri;
        vTri.a = glm::vec3(mod->vertices[mod->vertIdx[i * 3] * 3], -mod->vertices[mod->vertIdx[i * 3] * 3 + 1], mod->vertices[mod->vertIdx[i * 3] * 3 + 2]);
        vTri.b = glm::vec3(mod->vertices[mod->vertIdx[i * 3 + 1] * 3], -mod->vertices[mod->vertIdx[i * 3 + 1] * 3 + 1], mod->vertices[mod->vertIdx[i * 3 + 1] * 3 + 2]);
        vTri.c = glm::vec3(mod->vertices[mod->vertIdx[i * 3 + 2] * 3], -mod->vertices[mod->vertIdx[i * 3 + 2] * 3 + 1], mod->vertices[mod->vertIdx[i * 3 + 2] * 3 + 2]);
        nTri.a = glm::vec3(mod->normals[mod->normIdx[i * 3] * 3], -mod->normals[mod->normIdx[i * 3] * 3 + 1], mod->normals[mod->normIdx[i * 3] * 3 + 2]);
        nTri.b = glm::vec3(mod->normals[mod->normIdx[i * 3 + 1] * 3], -mod->normals[mod->normIdx[i * 3 + 1] * 3 + 1], mod->normals[mod->normIdx[i * 3 + 1] * 3 + 2]);
        nTri.b = glm::vec3(mod->normals[mod->normIdx[i * 3 + 2] * 3], -mod->normals[mod->normIdx[i * 3 + 2] * 3 + 1], mod->normals[mod->normIdx[i * 3 + 2] * 3 + 2]);
        tTri.a = glm::vec2(mod->texture[mod->texIdx[i * 3] * 2], mod->texture[mod->texIdx[i * 3] * 2 + 1]);
        tTri.b = glm::vec2(mod->texture[mod->texIdx[i * 3 + 1] * 2], mod->texture[mod->texIdx[i * 3 + 1] * 2 + 1]);
        tTri.c = glm::vec2(mod->texture[mod->texIdx[i * 3 + 2] * 2], mod->texture[mod->texIdx[i * 3 + 2] * 2 + 1]);
        vTri.a *= scale;
        vTri.b *= scale;
        vTri.c *= scale;
        vTri.a += this->origPoint;
        vTri.b += this->origPoint;
        vTri.c += this->origPoint;
        triangles[i].setTriangle(vTri);
        triangles[i].setTriNorms(nTri);
        triangles[i].setUVTriangle(tTri);
        triangles[i].setColor(cS, cD, cA);
        triangles[i].setGeometry(geo);
        triangles[i].shader = shader;
        triangles[i].setTexture(texture);
        triangles[i].getGroupNormal();
        triangles[i].getGroupPos();
    }

    setChildren(triangles, mod->idxLen / 3);
}
