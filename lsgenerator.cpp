#include "lsgenerator.h"

LSGenerator::LSGenerator()
{

}

void LSGenerator::stampLS(Grid<float> &g)
{
    printf("Starting LS Build\n");
    glm::vec3 LLC = g.getLLC();
    glm::vec3 URC = g.getURC();
    //printf("LLC: %f, %f, %f\n", LLC.x, LLC.y, LLC.z);
    //printf("URC: %f, %f, %f\n", URC.x, URC.y, URC.z);
    int arr[3];
    g.getDimensions(arr);
    int Nx = arr[0];
    int Ny = arr[1];
    int Nz = arr[2];

    for(int k = 0; k < Nz; k++)
    {
        for(int j = 0; j < Ny; j++)
        {
#pragma omp parallel for
            for(int i = 0; i < Nx; i++)
            {
                int idx = g.getIndex(i, j, k);
                g.setDataAt(idx, findClosestPoint(g.getIndexPos(i, j, k)));
            }
        }
        printf("building LS: %f\n", (k * (float)Nx * (float) Ny) / ((float)Nx * Ny * Nz));
    }


}

bool LSGenerator::readObj(char *fname)
{
    printf("----------------------loading Level Set OBJ----------------------\n");
    vecs.clear();
    norms.clear();
    indicies.clear();
    nindicies.clear();
    std::string hold;
    float hold1[3];
    int hold2[3];
    int hold21[3];
    std::ifstream in;
    in.open(fname);
    if(in.is_open())
    {
        while(!in.eof())
        {
            in >> hold;
            if(hold.compare("v") == 0)
            {
                in >> hold1[0] >> hold1[1] >> hold1[2];
                vecs.push_back(glm::vec3(hold1[0], hold1[1], hold1[2]));
            }
            if(hold.compare("vn") == 0)
            {
                in >> hold1[0] >> hold1[1] >> hold1[2];
                norms.push_back(glm::vec3(hold1[0], hold1[1], hold1[2]));
            }
            else if(hold.compare("f") == 0)
            {
                in >> hold2[0];// >> hold21[0] >> hold2[1] >> hold21[1] >> hold2[2] >> hold21[2];
                in.seekg(2, in.cur);
                in >> hold21[0] >> hold2[1];
                in.seekg(2, in.cur);
                in >> hold21[1] >> hold2[2];
                in.seekg(2, in.cur);
                in >> hold21[2];
                indicies.push_back(std::vector<int>{hold2[0]-1, hold2[1]-1, hold2[2]-1});
                nindicies.push_back(std::vector<int>{hold21[0]-1, hold21[1]-1, hold21[2]-1});
            }
            else
            {
                std::getline(in, hold);
            }
        }
    }
    else
    {
        printf("cannot open file %s", fname);
        return false;
    }
    printf("-------------------finished loading Level Set OBJ------------------\n");
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
        evaluateTri(pos, vecs[indicies[i][0]], vecs[indicies[i][1]], vecs[indicies[i][2]], norms[nindicies[i][0]], norms[nindicies[i][1]], norms[nindicies[i][2]], ptri, ntri);
        dist = glm::length(pos - ptri);
        //printf("dist: %f\n", dist);
        if(i == 0 || dist < minDist)
        {
            //printf("-----NEW DIST------\n");
            minDist = dist;
            closestPoint = ptri;
            nclosest = ntri;
        }
    }

    //printf("ntri: %f, %f, %f\n", ntri.x, ntri.y, ntri.z);
    //printf("nclosest: %f, %f, %f\n", nclosest.x, nclosest.y, nclosest.z);
    //printf("closestPoint: %f, %f, %f\n", closestPoint.x, closestPoint.y, closestPoint.z);
    //printf("pos: %f, %f, %f\n", pos.x, pos.y, pos.z);
    //printf("pos-closestPoint: %f, %f, %f\n", pos.x-closestPoint.x, pos.y-closestPoint.y, pos.z-closestPoint.z);
    //float inout = glm::dot(pos-closestPoint, nclosest);
    //printf("%f\n", inout);
    if(glm::dot(pos-closestPoint, nclosest) > 0)
    {
        minDist = -minDist;
    }
    //printf("%f\n", minDist);
    return minDist;
}



void LSGenerator::evaluateTri(glm::vec3 x, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 n0, glm::vec3 n1, glm::vec3 n2, glm::vec3& p, glm::vec3& n)
{
    //first check if the closest point is on the triangle itself.
    //printf("x: %f, %f, %f\n", x.x, x.y, x.z);
    //printf("p0: %f, %f, %f\n", p0.x, p0.y, p0.z);
    //printf("p1: %f, %f, %f\n", p1.x, p1.y, p1.z);
    //printf("p2: %f, %f, %f\n", p2.x, p2.y, p2.z);
    glm::vec3 e1 = p1 - p0;
    //printf("e1: %f, %f, %f\n", e1.x, e1.y, e1.z);
    glm::vec3 e2 = p2 - p0;
    //printf("e2: %f, %f, %f\n", e2.x, e2.y, e2.z);
    glm::vec3 e3 = p2 - p1;
    //printf("e3: %f, %f, %f\n", e3.x, e3.y, e3.z);
    n = glm::normalize(glm::cross(e1, e2));
    //printf("n: %f, %f, %f\n", n.x, n.y, n.z);
    glm::vec3 r = x - p0 - n * glm::dot(n, (x - p0));
    //printf("r: %f, %f, %f\n", r.x, r.y, r.z);
    glm::vec3 e12 = glm::cross(e1,e2);
    //printf("e12: %f, %f, %f\n", e12.x, e12.y, e12.z);
    glm::vec3 e21 = glm::cross(e2, e1);
    //printf("e21: %f, %f, %f\n", e21.x, e21.y, e21.z);
    float v = glm::dot(glm::cross(e1, r), e12) / (glm::length(e12) * glm::length(e12));
    float u = glm::dot(glm::cross(e2, r), e21) / (glm::length(e21) * glm::length(e21));
    if(u >= 0 && u <= 1 && v >= 0 && v <= 1 && (u + v) >= 0 && (u + v) <= 1)
    {
        p = p0 + u*e1 + v*e2;
        //printf("p: %f, %f, %f\n", p.x, p.y, p.z);
        return;
    }
    //if the closest point isn't on the face of the triangle, check the edges.
    float q = glm::dot(e1, (x - p0)) / (glm::length(e1) * glm::length(e1));
    if(q >= 0 && q <= 1)
    {
        p = p0 + q * e1;
        n = glm::normalize(n0 + n1);
        //printf("p: %f, %f, %f\n", p.x, p.y, p.z);
        return;
    }
    q = glm::dot(e2, (x - p0)) / (glm::length(e2) * glm::length(e2));
    if(q >= 0 && q <= 1)
    {
        p = p0 + q * e2;
        n = glm::normalize(n0 + n2);
        //printf("p: %f, %f, %f\n", p.x, p.y, p.z);
        return;
    }
    q = glm::dot(e3, (x - p1)) / (glm::length(e3) * glm::length(e3));
    if(q >= 0 && q <= 1)
    {
        p = p1 + q * e3;
        n = glm::normalize(n1 + n2);
        //printf("p: %f, %f, %f\n", p.x, p.y, p.z);
        return;
    }
    //if the closest point isn't on one of the lines, just get the closest vertex.
    float l0 = glm::length(x - p0);
    float l1 = glm::length(x - p1);
    float l2 = glm::length(x - p2);
    if(l0 <= l1 && l0 <= l2)
    {
        p = p0;
        n = glm::normalize(n0);
    }
    else if(l1 <= l0 && l1 <= l2)
    {
        p = p1;
        n = glm::normalize(n1);
    }
    else
    {
        p = p2;
        n = glm::normalize(n2);
    }
    //printf("p: %f, %f, %f\n", p.x, p.y, p.z);
    return;
}



























