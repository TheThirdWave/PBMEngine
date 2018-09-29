#ifndef GRID_H
#define GRID_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"

template < class T >
class Grid
{
public:
    Grid(glm::vec3 L, glm::vec3 U, int nx, int ny, int nz)
    {
        LLC = L;
        URC = U;
        Nx = nx;
        Ny = ny;
        Nz = nz;
        dx = (URC.x - LLC.x) / Nx;
        dy = (URC.y - LLC.y) / Ny;
        dz = (URC.z - LLC.z) / Nz;
        data = new T[Nx * Ny * Nz];
    };

    void setLLC(glm::vec3 l)
    {
        LLC = l;
    };

    void setURC(glm::vec3 u)
    {
        URC = u;
    };

    void setDataAt(int index, T d)
    {
        data[index] = d;
    };

    int getIndex(int i, int j, int k)
    {
        return i + Nx * j + Nx * Ny * k;
    };

    glm::vec3 getIndexPos(int i, int j, int k)
    {
        return LLC + glm::vec3(i * dx, j * dy, k * dz);
    };

    glm::vec3 getLLC()
    {
        return LLC;
    };

    glm::vec3 getURC()
    {
        return URC;
    };

    void getDimensions(int* arr)
    {
        arr[0] = Nx;
        arr[1] = Ny;
        arr[2] = Nz;
    }; //expects an int array of length 3 : int[3]

    T trilerp(const glm::vec3& x)
    {
        glm::vec3 Xg = x - LLC;
        int i = Xg.x / dx;
        int j = Xg.y / dy;
        int k = Xg.z / dz;
        T c000, c001, c010, c011, c100, c101, c110, c111, c00, c01, c10, c11, c0, c1;
        int index = getIndex(i, j, k);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c000 = data[index];
        index = getIndex(i, j, k + 1);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c001 = data[index];
        index = getIndex(i, j + 1, k);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c010 = data[index];
        index = getIndex(i, j + 1, k + 1);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c011 = data[index];
        index = getIndex(i + 1, j, k);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c100 = data[getIndex(i + 1, j, k)];
        index = getIndex(i + 1, j, k + 1);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c101 = data[getIndex(i + 1, j, k + 1)];
        index = getIndex(i + 1, j + 1, k);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c110 = data[index];
        index = getIndex(i + 1, j + 1, k + 1);
        if(index < 0 || index > Nx * Ny * Nz)
        {
            int x = 0;
        }
        c111 = data[index];
        c00 = c000 * (1 - Xg.x) + c100 * Xg.x;
        c01 = c001 * (1 - Xg.x) + c101 * Xg.x;
        c10 = c010 * (1 - Xg.x) + c110 * Xg.x;
        c11 = c011 * (1 - Xg.x) + c111 * Xg.x;
        c0 = c00 * (1 - Xg.y) + c10 * Xg.y;
        c1 = c01 * (1 - Xg.y) + c11 * Xg.y;
        return c0 * (1 - Xg.z) + c1 * Xg.z;
    };

private:
    glm::vec3 LLC;
    glm::vec3 URC;
    int Nx, Ny, Nz;
    float dx, dy, dz;
    T* data;
};

#endif // GRID_H
