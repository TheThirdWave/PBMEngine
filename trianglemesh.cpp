#include "trianglemesh.h"
#include "../glm-0.9.8.5/glm/gtx/rotate_vector.hpp"
#include "trianglefunction.h"

TriangleMesh::TriangleMesh()
{
}

glm::vec4 TriangleMesh::getTexCol(glm::vec3)
{

}

float TriangleMesh::getRelativePoint(glm::vec3)
{

}

int TriangleMesh::getRelativeLine(glm::vec3 pt, glm::vec3 nL, intercept* hits, int idx)
{
    for(int i = 0; i < numChildren; i++)
    {
        idx = children[i].getRelativeLine(pt, nL, hits, idx);
    }
    return idx;
}

glm::vec3 TriangleMesh::getSurfaceNormal(glm::vec3)
{

}

void TriangleMesh::createTetrahedron(float radius)
{
    glm::vec3 upVec = normal;
    glm::vec3 n0 = glm::normalize(glm::cross(normal, normal2));
    glm::vec3 points[4];
    points[0] = origPoint + glm::vec3(200.0f, 0.0f, 0.0f);//n0 * radius;
    glm::vec3 n1 = glm::rotate(n0, 60.0f, upVec);
    points[1] = origPoint + glm::vec3(-200.0f, 0.0f, 0.0f);//n1 * radius;
    points[2] = origPoint + glm::vec3(0.0f, 0.0f, 200.0f);//glm::rotate(n1, 60.0f, upVec) * radius;
    points[3] = origPoint + glm::vec3(0.0f, -200.0f, 0.0f);//upVec * radius;
    origPoint = (points[0] + points[1] + points[2] + points[3]) / 4.0f;
    glm::vec3 norms[4];
    for(int i = 0; i < 4; i++)
    {
        norms[i] = points[i] - origPoint;
    }
    TriangleFunction* triangles = new TriangleFunction[4];
    for(int i = 0; i < 4; i ++)
    {
        triangles[i];
        triangles[i].setColor(cS, cD, cA);
        triangles[i].setGeometry(geo);
        triangles[i].setTexture(texture);
        triangles[i].shader = shader;
    }
    triangles[0].setTriangle(points[0], points[1], points[2]);
    triangles[0].setTriNorms(norms[0], norms[1], norms[2]);
    triangles[0].getGroupNormal();
    triangles[0].getGroupPos();
    triangles[1].setTriangle(points[3], points[1], points[2]);
    triangles[1].setTriNorms(norms[0], norms[1], norms[2]);
    triangles[1].getGroupNormal();
    triangles[1].getGroupPos();
    triangles[2].setTriangle(points[0], points[1], points[2]);
    triangles[2].setTriNorms(norms[0], norms[1], norms[2]);
    triangles[2].getGroupNormal();
    triangles[2].getGroupPos();
    triangles[3].setTriangle(points[0], points[1], points[2]);
    triangles[3].setTriNorms(norms[0], norms[1], norms[2]);
    triangles[3].getGroupNormal();
    triangles[3].getGroupPos();

    setChildren(triangles, 4);
}
