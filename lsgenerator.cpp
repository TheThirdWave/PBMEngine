#include "lsgenerator.h"

LSGenerator::LSGenerator()
{

}

void LSGenerator::stampLS(Grid<float> &g)
{
    glm::vec3 LLC = g.getLLC();
    glm::vec3 URC = g.getURC();
    int arr[3];
    g.getDimensions(arr);
    int Nx = arr[0];
    int Ny = arr[1];
    int Nz = arr[2];
    float dx = (URC.x - LLC.x)/(float)Nx;
    float dy = (URC.y - LLC.y)/(float)Ny;
    float dz = (URC.z - LLC.z)/(float)Nz;
    int idx;

    for(int k = 0; k < Nz; k++)
    {
        for(int j = 0; j < Ny; j++)
        {
            for(int i = 0; i < Nx; i++)
            {
                idx = g.getIndex(i, j, k);
                g.setDataAt(idx, findClosestPoint(glm::vec3(LLC.x + dx * i, LLC.y + dy * j, LLC.z + dz * k)));
            }
        }
    }


}

bool LSGenerator::readObj(char *fname)
{
    char hold;
    float hold1[3];
    int hold2[3];
    std::ifstream in;
    in.open(fname);
    if(in.is_open())
    {
        in >> hold;
        if(hold == 'v')
        {
            in >> hold1[0] >> hold1[1] >> hold1[2];
            vecs.push_back(glm::vec3(hold1[0], hold1[1], hold1[2]));
        }
        else if(hold == 'f')
        {
            in >> hold2[0] >> hold2[1] >> hold2[2];
            indicies.push_back(std::vector<int>{hold2[0], hold2[1], hold2[2]});
        }
    }
    else
    {
        printf("cannot open file %s", fname);
        return false;
    }
    return true;
}

float LSGenerator::findClosestPoint(glm::vec3 pos)
{
    //loop through all triangles.
    float minDist;
    float dist;
    glm::vec3 ptri;
    glm::vec3 ntri;
    glm::vec3 closestPoint;
    glm::vec3 nclosest;
    for(int i = 0; i < indicies.size(); i++)
    {
        evaluateTri(pos, vecs[indicies[i][0]], vecs[indicies[i][1]], vecs[indicies[i][1]], ptri, ntri);
        dist = glm::length(pos - ptri);
        if(i == 0 || dist < minDist)
        {
            minDist = dist;
            closestPoint = ptri;
            nclosest = ntri;
        }
    }

    //printf("%f\n", minDist);
    if(glm::dot(pos, nclosest) > 0) return -minDist;
    return minDist;
}

void LSGenerator::evaluateTri(glm::vec3 x, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3& p, glm::vec3& n)
{
    //first check if the closest point is on the triangle itself.
    glm::vec3 e1 = p1 - p0;
    glm::vec3 e2 = p2 - p0;
    glm::vec3 e3 = p2 - p1;
    n = glm::normalize(glm::cross(e1, e2));
    glm::vec3 r = x - p0 - n * glm::dot(n, (x - p0));
    glm::vec3 e12 = glm::cross(e1,e2);
    glm::vec3 e21 = glm::cross(e2, e1);
    float v = glm::dot(glm::cross(e1, r), e12) / (glm::length(e12) * glm::length(e12));
    float u = glm::dot(glm::cross(e2, r), e21) / (glm::length(e21) * glm::length(e21));
    if(u >= 0 && u <= 1 && v >= 0 && v <= 1 && (u + v) >= 0 && (u + v) <= 1)
    {
        p = p0 + u*e1 + v*e2;
        return;
    }
    //if the closest point isn't on the face of the triangle, check the edges.
    float q = glm::dot(e1, (x - p0)) / (glm::length(e1) * glm::length(e1));
    if(q >= 0 && q <= 1)
    {
        p = p0 + q * e1;
        return;
    }
    q = glm::dot(e2, (x - p0)) / (glm::length(e2) * glm::length(e2));
    if(q >= 0 && q <= 1)
    {
        p = p0 + q * e2;
        return;
    }
    q = glm::dot(e3, (x - p1)) / (glm::length(e3) * glm::length(e3));
    if(q >= 0 && q <= 1)
    {
        p = p1 + q * e3;
        return;
    }
    //if the closest point isn't on one of the lines, just get the closest vertex.
    float l0 = glm::length(x - p0);
    float l1 = glm::length(x - p1);
    float l2 = glm::length(x - p2);
    if(l0 <= l1 && l0 <= l2) p = p0;
    else if(l1 <= l0 && l1 <= l2) p = p1;
    else p = p2;
    return;
}



























